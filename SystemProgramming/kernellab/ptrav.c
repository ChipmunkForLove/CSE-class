//------------------------------------------------------------------------------
/// @brief Kernel Lab: Page Table Walk (dbfs_ptrav.c)
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

struct packet {
        pid_t pid;
        unsigned long pages_1g, pages_2m, pages_4k;
};

static struct dentry *dir, *output;
static struct task_struct *task;

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
	pid_t pid;
	int i,j,k,m;
        copy_from_user(&pckt ,user_buffer ,sizeof(struct packet));
	pid=pckt.pid;
	pckt.pages_1g=0;
	pckt.pages_2m=0;
	pckt.pages_4k=0;
	task=pid_task(find_vpid(pid),PIDTYPE_PID);
	pgdp=task->mm->pgd;
	for(i=0;i<512;i++){//has 512 page entries
		if(pgd_present(pgdp[i])){
			pudp=(pud_t*)(&pgdp[i]);
			for(j=0;j<512;j++){
				if(pud_present(pudp[j])){
					//check if it is 1GB page
					if(pud_large(pudp[j])){
						pckt.pages_1g++;
					}
					else{
					pmdp=(pmd_t*)(&pudp[j]);
						for(k=0;k<512;k++){
							if(pmd_present(pmdp[k])){
								//check if it is 2MB page
								if(pmd_large(pmdp[k])){
									pckt.pages_2m++;
								}
								else{
								ptep=(pte_t*)(&pmdp[k]);
									for(m=0;m<512;m++){
										if(pte_present(ptep[m])){
											pckt.pages_4k++;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	


        // Fill in the arguments below
        copy_to_user(user_buffer,&pckt,sizeof(struct packet) );

        return length;
}

static const struct file_operations dbfs_fops = {
        // Mapping file operations with your functions
	.read=read_output,
};

static int __init dbfs_module_init(void)
{
        // Implement init module
        printk("Init Module\n");

        dir = debugfs_create_dir("ptrav", NULL);

        if (!dir) {
                printk("Cannot create paddr dir\n");
                return -1;
        }

        // Fill in the arguments below
        output = debugfs_create_file("output",0444,dir,NULL,&dbfs_fops);

        return 0;
}

static void __exit dbfs_module_exit(void)
{
	debugfs_remove_recursive(dir);
        // Implement exit module
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
