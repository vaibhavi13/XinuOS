#ifndef MT

#include <xinu.h>
#include <mmu.h>

#else

#include <stdlib.h>
#include <stdio.h>
#include "../../../include/memmax.h"
#include "../../../include/platform/arm/armv7a.h"

#endif


/* These constants are based on the 1MB Section descriptor format
 * described in Ffigure 5.6 of this:
 * https://developer.arm.com/documentation/ddi0360/f/memory-management-unit/hardware-page-table-translation/armv6-page-table-translation-subpage-ap-bits-disabled
 */

#ifndef NPROC
#define NPROC 4
#endif

#define MEM_SIZE  0x20000000   /* 512MB */
#define SECTION_SIZE 0x100000 /* 1MB */

#ifdef MT
//#define NUM_PAGE_TABLE_ENTRIES 0x1000 /* 4K, 4096 */
#define NUM_PAGE_TABLE_ENTRIES 256
#endif

#define SDRAM_START       0x80000000
#define SDRAM_END         0x8fffffff

#define SECTION_BASE      0x02        /* b10 this is a section base, not PT base */
#define CACHE_ENABLE      0x10
#define CACHE_WRITEBACK   0x1e
#define AP_RW 0x0c00   /* this was 3 << 10 -- bits 10 and 11, RW access permission */

#define BREAK 128
#define NUM_PAGES_PER_PROC 2

#ifndef MT

uint32 __attribute__((aligned(16384))) page_table[NPROC][NUM_PAGE_TABLE_ENTRIES];

#else

uint32_t page_table[NPROC][NUM_PAGE_TABLE_ENTRIES];

#endif

#ifndef MT

void initialize_mmu(void) {

#else

int main(int argc, char *argv[]) {

#endif

    int pg;
    int asid;
    int page_base;
    int page_base_target;
    int pg_to_map;
    int pg_to_map_to;
    int pg_proc_offset;
#ifdef COMP
    int region_copies;
#endif

    /* Identity mappings 
       shift << 20 for 1M "section base" type mappings 
    */
    for (asid = 0; asid < NPROC; asid++){
      for (pg = 0; pg < NUM_PAGE_TABLE_ENTRIES; pg++){ 
        page_table[asid][pg] = (pg << 20 | AP_RW | CACHE_ENABLE | SECTION_BASE);
        
#ifdef MT
        printf("pt[%d][%08x]: %08x\n", asid, pg*1024*1024, page_table[asid][pg]);
#endif
      }
    }

#ifdef MT
    printf("-------------------------\n");
#endif    
    
    /* Per process mappings in here, starting at BREAK */
    page_base = BREAK;
    page_base_target = BREAK;
#ifdef COMP
    for (region_copies = 0; region_copies < NPROC; region_copies++ ) {
#endif      
      for (pg = 0; pg < NUM_PAGES_PER_PROC; pg++){
        for (asid = 0; asid < NPROC; asid++){
          pg_to_map = page_base + pg;
          pg_to_map_to = page_base_target + pg;
          pg_proc_offset = (pg * (NPROC-1));
#ifdef MT
          //printf("pg_to_map(%x), page_base(%x) + pg(%x) + pg_proc_offset(%x) -- page_base_target(%x), pg_to_map_to(%x)\n", 
          //       pg_to_map, page_base, pg, pg_proc_offset, page_base_target, pg_to_map_to);
#endif
          page_table[asid][pg_to_map] = 
            ((pg_to_map_to + asid + pg_proc_offset) << 20 | AP_RW | CACHE_ENABLE | SECTION_BASE);
        
#ifdef MT
          printf("pt[%d][%08x]: %08x\n", asid, pg_to_map*1024*1024, page_table[asid][pg_to_map]);        
#endif
        }
      }
#ifdef COMP      
      page_base += NUM_PAGES_PER_PROC;
      }
#endif    
#ifdef MT    
}
#else

 set_page_table();    
 flush_tlb();
 set_access_control();
 enable_mmu(); 
}

 
 void set_page_table(){
	uint32 pagetable =(uint32) page_table[currpid];
	/* Copy the page table address to cp15 */
    asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r" (pagetable) : "memory");
 }

void set_access_control(){
/* Set the access control to all-supervisor */
    asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (~0));
}

void flush_tlb(){
/* Set the access control to all-supervisor */
  asm volatile("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
}

void enable_mmu(){
    uint32 reg;
/* Enable the MMU */
    asm("mrc p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
    reg |= ARMV7A_C1CTL_M;
    asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (reg) : "cc");
}

void page_fault_handler(){
	kprintf("Segmentation Fault\n");
}

#endif
