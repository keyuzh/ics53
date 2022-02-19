#ifndef HELPERS_H
#define HELPERS_H

#include "icsmm.h"

void initializeHeap(ics_free_header** head);

size_t calculateRequiredBlockSize(size_t size);

void* allocateBlock(size_t requested_size, size_t block_size, ics_free_header* block, ics_free_header** freelist_head);



int checkBlockValid(void* payload_addr);
int checkAllocated(void* word);
// int checkBlockIsAllocated(ics_header* header, ics_footer* footer);
int checkHeaderField(ics_header* header);
int checkFootererField(ics_footer* footer);
int checkSizeEqual(ics_header* header, ics_footer* footer);

void freeBlock(void* payload_addr, ics_free_header** freelist_head);

void removeBlock(ics_free_header* block, ics_free_header** freelist_head);

void addBlockToFreeList(ics_free_header* block, ics_free_header** freelist_head);

#endif
