#----------------------------------------------------------------
#
#  4190.308 Computer Architecture (Fall 2019)
#
#  Project #4: A 3-Stage Pipelined RISC-V Simulator
#
#  November 19, 2019
#
#  Jin-Soo Kim (jinsoo.kim@snu.ac.kr)
#  Systems Software & Architecture Laboratory
#  Dept. of Computer Science and Engineering
#  Seoul National University
#
#----------------------------------------------------------------

from consts import *
from isa import *
from program import *
from components import *
from stages import *


#--------------------------------------------------------------------------
#   Pipeline implementation-specific constants
#--------------------------------------------------------------------------

S_FD      = 0
S_EX      = 1
S_MW      = 2

S = [ 'FD', 'EX', 'MW' ]


#--------------------------------------------------------------------------
#   Pipe: manages overall execution with logging support
#--------------------------------------------------------------------------

class Pipe(object):

    def __init__(self):
        self.name = self.__class__.__name__

    @staticmethod
    def set_stages(cpu, stages, ctl):
        Pipe.cpu = cpu
        Pipe.stages = stages
        Pipe.FD = stages[S_FD]
        Pipe.EX = stages[S_EX]
        Pipe.MW = stages[S_MW]
        Pipe.CTL = ctl

    @staticmethod
    def run(entry_point):
        from stages import FD

        FD.reg_pc = entry_point
        while True:
            # Run each stage 
            # Should be run in the reverse order because forwarding and 
            # hazard control logic depends on previous instructions
            Pipe.MW.compute()
            Pipe.EX.compute()
            Pipe.FD.compute()

            # Update states
            Pipe.FD.update()
            Pipe.EX.update()
            ok = Pipe.MW.update()

            Stat.cycle      += 1
            if Pipe.MW.inst != BUBBLE:
                Stat.icount += 1
                opcode = RISCV.opcode(Pipe.MW.inst)
                if isa[opcode][IN_CLASS] == CL_ALU:
                    Stat.inst_alu += 1
                elif isa[opcode][IN_CLASS] == CL_MEM:
                    Stat.inst_mem += 1
                elif isa[opcode][IN_CLASS] == CL_CTRL:
                    Stat.inst_ctrl += 1

            # Show logs after executing a single instruction
            if Log.level >= 6:
                Pipe.cpu.rf.dump()                      # dump register file
            if Log.level >= 7:
                Pipe.cpu.dmem.dump(skipzero = True)     # dump dmem

            if not ok:
                break;

        # Handle exceptions, if any
        if (Pipe.MW.exception & EXC_DMEM_ERROR):
            print("Exception '%s' occurred at 0x%08x -- Program terminated" % (EXC_MSG[EXC_DMEM_ERROR], Pipe.MW.pc))
        elif (Pipe.MW.exception & EXC_ECALL):
            print("Execution completed")
        elif (Pipe.MW.exception & EXC_ILLEGAL_INST):
            print("Exception '%s' occurred at 0x%08x -- Program terminated" % (EXC_MSG[EXC_ILLEGAL_INST], Pipe.MW.pc))
        elif (Pipe.MW.exception & EXC_IMEM_ERROR):
            print("Exception '%s' occurred at 0x%08x -- Program terminated" % (EXC_MSG[EXC_IMEM_ERROR], Pipe.MW.pc))

        if Log.level > 0:
            if Log.level < 6:
                Pipe.cpu.rf.dump()                      # dump register file
            if Log.level > 1 and Log.level < 7:
                Pipe.cpu.dmem.dump(skipzero = True)     # dump dmem
       
    # This function is called by each stage after updating its states
    @staticmethod
    def log(stage, pc, inst, info):

        if Stat.cycle < Log.start_cycle:
            return
        if Log.level >= 4 and stage == S_FD:
            print("-" * 50)
        if Log.level < 5:
            info = ''
        if Log.level >= 4 or (Log.level == 3 and stage == S_MW):
            print("%d [%s] 0x%08x: %-30s%-s" % (Stat.cycle, S[stage], pc, Program.disasm(pc, inst), info))
        else:
            return

