#==========================================================================
# 4190.308 Computer Architecture                               Spring 2020
#
# Cache Lab
#
# Handou:   June  8, 2020
# Due:      June 22, 2020
#
# Implement your cache version. The dummy version provided simply forwards
# the read/write access to the next level in the memory hierachy. Fix it
# in order to do some real caching. Look at the Cache class in this file.
# Your implementation should produce correct results and greatly improve
# performance (lower CPI). Cache acceses should return the actual cycle
# count it took to process them (cache cost + eventual cost of accessing the
# next level).
# NOTE: In the simulator, memory is accessed by word (32bits). When
# transferring a cache block from or to memory, you can count the access
# cost a single time ( you don't need to count each accessed word
# individually).
# The dummy implementation accessed a single byte of the next level memory.
# In your implementaion, this should be avoided. Transfer entire blocks.
#
#==========================================================================
#
#   The PyRISC Project
#
#   SNURISC5: A 5-stage Pipelined RISC-V ISA Simulator
#
#   Classes for hardware components: RegisterFile, Memory, Adder, etc.
#
#   Jin-Soo Kim
#   Systems Software and Architecture Laboratory
#   Seoul National University
#   http://csl.snu.ac.kr
#
#==========================================================================

from consts import *
from isa import *


#--------------------------------------------------------------------------
#   Constants
#--------------------------------------------------------------------------

# Symbolic register names
rname =  [ 
            'zero', 'ra',  'sp',  'gp',  'tp',  't0',  't1',  't2',
            's0',   's1',  'a0',  'a1',  'a2',  'a3',  'a4',  'a5',
            'a6',   'a7',  's2',  's3',  's4',  's5',  's6',  's7',
            's8',   's9',  's10', 's11', 't3',  't4',  't5',  't6' 
        ]


#--------------------------------------------------------------------------
#   RegisterFile: models 32-bit RISC-V register file
#--------------------------------------------------------------------------

class RegisterFile(object):

    def __init__(self):
        self.reg = WORD([0] * NUM_REGS)

    def read(self, regno):

        if regno == 0:
            return 0
        elif regno > 0 and regno < NUM_REGS:
            return self.reg[regno]
        else:
            raise ValueError

    def write(self, regno, value):

        if regno == 0:
            return
        elif regno > 0 and regno < NUM_REGS:
            self.reg[regno] = WORD(value)
        else:
            raise ValueError

    def dump(self, columns = 4):

        print("Registers")
        print("=" * 9)
        for c in range (0, NUM_REGS, columns):
            str = ""
            for r in range (c, min(NUM_REGS, c + columns)):
                name = rname[r]
                val = self.reg[r]
                str += "%-11s0x%08x    " % ("%s ($%d):" % (name, r), val)
            print(str)


#--------------------------------------------------------------------------
#   Register: models a single 32-bit register
#--------------------------------------------------------------------------

class Register(object):

    def __init__(self, initval = 0):
        self.r = WORD(initval)

    def read(self):
        return self.r

    def write(self, val):
        self.r = WORD(val)


#--------------------------------------------------------------------------
#   Memory: models a memory
#--------------------------------------------------------------------------

class Memory(object):

    def __init__(self, mem_start, mem_size, word_size, cost):
        self.word_size  = word_size
        self.mem_words  = mem_size // word_size
        self.mem_start  = mem_start
        self.mem_end    = mem_start + mem_size
        self.mem        = WORD([0] * self.mem_words)
        self.cost       = cost

    def access(self, valid, addr, data, fcn):

        if (not valid):
            res = [ WORD(0), True, 0 ]
        elif (WORD(addr) < self.mem_start) or (WORD(addr) > self.mem_end) or \
            addr % self.word_size != 0:
            res = [ WORD(0) , False, 0 ]
        elif fcn == M_XRD:
            val = self.mem[(addr - self.mem_start) // self.word_size]
            res = [ val, True, self.cost ]
        elif fcn == M_XWR:
            self.mem[(addr - self.mem_start) // self.word_size] = WORD(data)
            res = [ WORD(0), True, self.cost ]
        else:
            res = [ WORD(0), False, 0 ]

        return res

    def dump(self, skipzero = False):

        print("Memory 0x%08x - 0x%08x" % (self.mem_start, self.mem_end - 1))
        print("=" * 30)
        for a in range(self.mem_start, self.mem_end, self.word_size):
            val, status, cost = self.access(True, a, 0, M_XRD)
            if not status:
                continue
            if (not skipzero) or (val != 0):
                print("0x%08x: " % a, ' '.join("%02x" % ((val >> i) & 0xff) for i in [0, 8, 16, 24]), " (0x%08x)" % val)


#--------------------------------------------------------------------------
#   Cache: models a cache
#--------------------------------------------------------------------------

STUDENT_ID = '2014-11540'
NAME = 'Yoonyoung Kweon'

class Entry(object):
    def __init__(self, blk_size, valid = False, tag = WORD(0)):
        self.valid = valid
        self.dirty = False
        self.tag = tag
        self.blk = [WORD(0)] * (blk_size // WORD_SIZE)
        self.LRU =0 #check how many times this entry used before.

class Cache(object):

    # Initialize cache
    # s:       number of bits used as set index
    # E:       number of entries per set (associativity)
    # b:       number of bits used as block offset index
    # cost:    cache access time (in cycles)
    # nxt_lvl: next level in the memory hierarchy
    def __init__(self, s, E, b, cost, nxt_lvl):
        self.s = s
        self.E = E 
        self.b = b 
        self.cost = cost
        self.nxt_lvl = nxt_lvl
        self.accesses = 0
        self.misses = 0
        self.hits = 0
        self.S = 2**s 
        self.B = 2**b
        self.tag_mask = WORD(~WORD(0) << (self.s + self.b))
        self.set_mask = WORD(~WORD(0) >> (WORD_SIZE * 8 - self.s) << self.b)
        self.blk_mask = WORD(~WORD(0) >> (WORD_SIZE * 8 - self.b))
        # sets
        self.sets = []
        for s in range(self.S):
            entries=[]
            for e in range(self.E): # each set has E number of entries now.
                entry = Entry(self.B)
                entries.append(entry)
            self.sets.append(entries)
    # Cache access (same as memory access)
    # valid:  is it a real request or just a place holder
    # addr:   address being accessed
    # data:   data to write
    # fcn:    read or write access ?
    # Returns the following tuple
    # data:   data read
    # status: True on success, False on error
    # cost:   access time
    def access(self, valid, addr, data, fcn):
        # "fake" access
        if (not valid):
            res = [ WORD(0), True, 0 ]
        # addr not aligned
        elif addr % WORD_SIZE != 0:
            res = [ WORD(0), False, 0 ]
        # IMPLEMENT YOUR CACHE HERE
        # rd/wr access
        elif fcn == M_XRD or fcn == M_XWR:
            self.accesses += 1
            tag = self.get_tag(addr)
            set_id = self.get_set(addr)
            entries = self.sets[set_id] # get entry list
            cost = self.cost
            min_entry=entries[0]
            e_index=0
            for e in range(self.E):
                e_index+=1
                if entries[e].valid and entries[e].tag == tag:
                    self.hits += 1
                    break
            if e_index==self.E:
                self.misses += 1
                # save in the entry least recently used
                entry = Entry(self.B, True, tag)
                min_entry=self.find_LRU_entry_save(entries,entry) # find least Recently used entry and save new entry there
                cost += self.rd_blk(min_entry, addr)
            blk_offset = addr & self.blk_mask
            if fcn == M_XRD:
                val = min_entry.blk[blk_offset // 4]
                res = [val, True, cost]
            else: # M_XWR
                min_entry.blk[blk_offset // 4] = data
                cost += self.wr_blk(min_entry, addr)
                res = [WORD(0), True, cost]
        # CACHE PART END
        # invalid access
        else:
            res = [ WORD(0), False, 0 ]
        return res
    def find_LRU_entry_save(self,entries,entry): 
        idx=0 
        min_idx=0
        minimum=100
        for e in entries: # find least used entry
            if e.LRU < minimum:
                minimum = e.LRU
                min_idx=idx
            idx+=1
        entries[min_idx]=entry
        entries[min_idx].LRU+=1
        return entries[min_idx]
    def get_set(self, addr):
        return (addr & self.set_mask) >> self.b

    def get_tag(self, addr):
        return (addr & self.tag_mask) >> (self.s + self.b)

    def rd_blk(self, entry, addr):
        blk_addr = addr & ~self.blk_mask
        for i in range(self.B // WORD_SIZE):
            data, _, _ = self.nxt_lvl.access(True, blk_addr + i * WORD_SIZE, WORD(0), M_XRD)
            entry.blk[i] = data
        return self.nxt_lvl.cost

    def wr_blk(self, entry, addr):
        blk_addr = addr & ~self.blk_mask
        for i in range(self.B // WORD_SIZE):
            _, _, _ = self.nxt_lvl.access(True, blk_addr + i * WORD_SIZE, entry.blk[i], M_XWR)
        return self.nxt_lvl.cost
           
 
    # print cache stats at the end of the execution
    def stats(self, name):
        print(name)
        print('  accesses: {:6}'.format(self.accesses))
        if self.accesses:
            print('  misses:   {:6} ({:3.0f}%)'.format(self.misses, self.misses * 100 / self.accesses))
            print('  hits:     {:6} ({:3.0f}%)'.format(self.hits, self.hits * 100 / self.accesses))
        else:
            print('  misses:   {:6}'.format(self.misses, 0))
            print('  hits:     {:6}'.format(self.hits, 0))

    # memory dump used by the simulator
    def dump(self, skipzero = False):
        self.nxt_lvl.dump(skipzero)


#--------------------------------------------------------------------------
#   ALU: models an ALU
#--------------------------------------------------------------------------

class ALU(object):

    def __init__(self):
        pass

    def op(self, alufun, alu1, alu2):

        np.seterr(all='ignore')
        if alufun == ALU_ADD:
            output = WORD(alu1 + alu2)
        elif alufun == ALU_SUB:
            output = WORD(alu1 - alu2)
        elif alufun == ALU_AND:
            output = WORD(alu1 & alu2)
        elif alufun == ALU_OR:
            output = WORD(alu1 | alu2)
        elif alufun == ALU_XOR:
            output = WORD(alu1 ^ alu2)
        elif alufun == ALU_SLT:
            output = WORD(1) if SWORD(alu1) < SWORD(alu2) else WORD(0)
        elif alufun == ALU_SLTU:
            output = WORD(1) if alu1 < alu2 else WORD(0)
        elif alufun == ALU_SLL:
            output = WORD(alu1 << (alu2 & 0x1f))
        elif alufun == ALU_SRA:
            output = WORD(SWORD(alu1) >> (alu2 & 0x1f))
        elif alufun == ALU_SRL:
            output = alu1 >> (alu2 & 0x1f)
        elif alufun == ALU_COPY1:
            output = alu1
        elif alufun == ALU_COPY2:
            output = alu2
        elif alufun == ALU_SEQ:
            output = WORD(1) if (alu1 == alu2) else WORD(0)
        else:
            output = WORD(0)

        return output


#--------------------------------------------------------------------------
#   Adder: models a simple 32-bit adder
#--------------------------------------------------------------------------

class Adder(object):

    def __init__(self):
        pass

    def op(self, operand1, operand2 = 4):
        np.seterr(all='ignore')
        return WORD(operand1 + operand2)


