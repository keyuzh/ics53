#ifndef HELPERS_H
#define HELPERS_H

#include "icsmm.h"

#define MAX_ALLOC_SIZE 20448

typedef struct memory_boundries
{
    void* start;
    void* end;
} memory_boundries;

size_t getBlockSize(void* word);
size_t getRequestedSize(void* word);
void setRequestedSize(void* word, size_t size);
void make_header(void* addr, size_t requested_size, size_t block_size, int allocated);
void make_footer(void* addr, size_t requested_size, size_t block_size, int allocated);
void make_header_and_footer(void* head_addr, size_t requested_size, size_t block_size, int allocated);
ics_footer* getFooterAddr(ics_header* header);
ics_header* getHeaderAddr(ics_footer* footer);

int getMorePage(int firstPage, memory_boundries* heap_boundaries);
size_t calculateRequiredBlockSize(size_t size);

void* allocateBlock(size_t requested_size, size_t block_size, ics_free_header* block);

int checkBlockValid(void* payload_addr, memory_boundries* bound);
int checkAddressValid(void* addr, memory_boundries* bound);
int checkAllocated(void* word);
int checkHeaderField(ics_header* header);
int checkFootererField(ics_footer* footer);
int checkSizeEqual(ics_header* header, ics_footer* footer);

void freeBlock(void* payload_addr);

void removeBlock(ics_free_header* block);

void addBlockToFreeList(ics_free_header* block);

ics_free_header* coalesce(ics_free_header* block);

void changeAllocatedBit(void* word, int a);

#endif
