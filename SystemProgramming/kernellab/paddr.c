//------------------------------------------------------------------------------
/// @brief Kernel Lab: VA-PA Translation (dbfs_paddr.c)
/// 
/// @section license_section License
/// Copyright (c) 2018-2019, Computer Systems and Platforms Laboratory, SNU
/// All rights reserved.
///
/// Redistribution and use in source and binary forms,  with or without modifi-
/// cation, are permitted provided that the following conditions are met:
///
/// - Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
/// - Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY  AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER  OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSE-
/// QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE
/// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
/// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT
/// LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY
/// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
/// DAMAGE.
//------------------------------------------------------------------------------
#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

MODULE_LICENSE("Dual BSD/GPL");

static struct dentry *dir, *output;
static struct task_struct *task;
struct packet{
	pid_t pid;
	unsigned long vaddr;
	unsigned long paddr;
};
static ssize_t read_output(struct file *fp,
                        char __user *user_buffer,
                        size_t length,
                        loff_t *position)
{
        // Implement read file operations
        // Fill in the arguments below
	struct packet pckt;
	pgd_t *pgdp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;
	phys_addr_t pfn;
	unsigned long vaddr=0;
	pid_t pid;
	copy_from_user(&pckt,user_buffer,sizeof(struct packet));
	pid=pckt.pid;
	vaddr=pckt.vaddr;
	task=pid_task(find_vpid(pid),PIDTYPE_PID);
	pgdp=pgd_offset(task->mm,vaddr);
	pudp=pud_offset(pgdp,vaddr);
	pmdp=pmd_offset(pudp,vaddr);
	ptep=pte_offset_kernel(pmdp,vaddr);
	pfn=pte_pfn(*ptep);
	pckt.paddr=pfn<<12;
        // Fill in the arguments below
        copy_to_user(user_buffer,&pckt,sizeof(struct packet));

        return length;
}

static const struct file_operations dbfs_fops = {
        // Mapping file operations with your functions
	.read=read_output,
};

static int __init dbfs_module_init(void)
{
        // Implement init module

        dir = debugfs_create_dir("paddr", NULL);

        if (!dir) {
                printk("Cannot create paddr dir\n");
                return -1;
        }

        // Fill in the arguments below
        output = debugfs_create_file("output",00700,dir,NULL,&dbfs_fops);

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module
	debugfs_remove_recursive(dir);
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
