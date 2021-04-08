#ifndef _MMU_H
#define _MMU_H

#define NUM_PAGE_TABLE_ENTRIES 4096 /* 1 entry per 1MB, so this covers 4G address space */


void initialize_mmu(void);
void setPageTable();
void setAccessControl();
void enableMMU();
void FlushTLB();
void pageFaultHandler();
#endif
