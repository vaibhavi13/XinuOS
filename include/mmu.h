#ifndef _MMU_H
#define _MMU_H

//#define NUM_PAGE_TABLE_ENTRIES 4096 /* 1 entry per 1MB, so this covers 4G address space */


void initialize_mmu(void);
void set_page_table(void);
void set_access_control(void);
void enable_mmu(void);
void flush_tlb(void);
void page_fault_handler(void);
#endif
