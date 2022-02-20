#include "helpers.h"
#include "debug.h"


/* Helper function definitions go here */

size_t getBlockSize(void* word)
{
    return ((ics_header*)word)->block_size & -2;
}

void make_header(void* addr, size_t requested_size, size_t block_size, int allocated)
{
    ics_header* header = (ics_header*) addr;
    *header = (ics_header) {block_size | allocated, HEADER_MAGIC, requested_size};
}

void make_footer(void* addr, size_t requested_size, size_t block_size, int allocated)
{
    ics_footer* footer = (ics_footer*) addr;
    *footer = (ics_footer) {block_size | allocated, FOOTER_MAGIC, requested_size};
}

void make_header_and_footer(void* head_addr, size_t requested_size, size_t block_size, int allocated)
{
    make_header(head_addr, requested_size, block_size, allocated);
    make_footer(getFooterAddr(head_addr), requested_size, block_size, allocated);
}

ics_footer* getFooterAddr(ics_header* header)
{
    return (ics_footer*)((void*)header + getBlockSize(header) - 8);
}

ics_header* getHeaderAddr(ics_footer* footer)
{
    return (ics_header*)((void*)footer + 8 - getBlockSize(footer));
}

// helpers for malloc()

int getMorePage(int firstPage, memory_boundries* heap_boundaries)
{
    // get start and end pointers of new page
    void* start = ics_inc_brk();
    if (start == (void*)-1)
    {
        // no more memory possible, errno should already be set
        return -1;
    }
    void* end = ics_get_brk();
    
    // create epilogue
    make_header(end-8, 0, 0, 1);
    heap_boundaries->end = end;

    // if first page, create prologue
    if (firstPage)
    {
        make_footer(start, 0, 0, 1);
        heap_boundaries->start = start;
    }

    // make a big free block
    // the location of header is either: 
    //   right after prologue (start + 8) if first page
    //   the old epilogue (start - 8) if subsequent pages
    void* head = start + (firstPage ? 8 : -8);
    void* foot = end - 16;

    // size of big free block if first page = 4096 - 8 (prologue) - 8 (epilogue)
    // subsequent pages = 4096 - 8 (epilogue) + 8 (prev epilogue)
    // or simply calculate the difference between addresses
    size_t block_size = foot - head + 8;
    make_header(head, 0, block_size, 0);
    make_footer(foot, 0, block_size, 0);

    // coalesce if possible then add the block to the free list
    ics_free_header* block = coalesce(head);
    addBlockToFreeList(block);
    
    return 0;
}

/*
    given the size of malloc call, calculate the block size that is required
    on the heap
*/
size_t calculateRequiredBlockSize(size_t size)
{
    // payload size = request size + (16 - (request size %16))    no need +16 if already multiple of 16
    // total block size = payload size + 8(header) + 8(footer)
    return size + (size % 16 == 0 ? 16 : 32) - (size % 16);
}

/*
    allocate the block, modify head pointer if needed
*/
void* allocateBlock(size_t requested_size, size_t block_size, ics_free_header* block)
{
    // bytes remaining in the free block after allocating
    size_t remaining_size = block->header.block_size - block_size;
    if (remaining_size < 32)
    {
        // if the remaining block is too small, add it to this block
        block_size += remaining_size;
    }
    else  // split block
    {
        // move the header to the splited block
        ics_free_header* new_header = (ics_free_header*) (((void*)block) + block_size);
        make_header((void*)new_header, 0, remaining_size, 0);
        // find the address of footer and update
        getFooterAddr((ics_header*)block)->block_size = remaining_size;
        // move the prev and next pointer
        new_header->next = block->next;
        new_header->prev = block->prev;
        // if prev pointer is null, it means that the block is the first block
        // of the explicit list, move the freelist_head pointer as well
        if (new_header->prev == NULL)
        {
            freelist_head = new_header;
        }
        else
        {
            // prev pointer not null, move the next pointer there to point at new header
            new_header->prev->next = new_header;
        }
        if (new_header->next != NULL)
        {
            // next pointer not null, move the prev pointer there to point at new header
            new_header->next->prev = new_header;
        }
    }
    // create header and footer for the allocated block
    make_header_and_footer((void*)block, requested_size, block_size, 1);

    // return the address of the new block
    return (void*)block;
}

// helpers for free()

int checkBlockValid(void* payload_addr, memory_boundries* bound)
{
    void* header = payload_addr - 8;
    if (checkAddressValid(header, bound) == 0)
    {
        // address out of bounds
        return 0;
    }
    ics_footer* footer = getFooterAddr(header);

    return (
        checkAllocated(header)
        && checkAllocated((void*)footer)  // allocated bit 'a' is set in both header and footer
        && checkHeaderField(header)       // hid field of header
        && checkFootererField(footer)     // fid field of footer
        && checkSizeEqual(header, footer) // block and requested size in header and footer are equal
    );
}

int checkAddressValid(void* addr, memory_boundries* bound)
{
    return ((addr > bound->start) && (addr < bound->end));
}

int checkAllocated(void* word)
{
    return (((ics_header*)word)->block_size & 1);
}

int checkHeaderField(ics_header* header)
{
    return (header->hid == HEADER_MAGIC);
}

int checkFootererField(ics_footer* footer)
{
    return (footer->fid == FOOTER_MAGIC);
}

int checkSizeEqual(ics_header* header, ics_footer* footer)
{
    return ((header->block_size == footer->block_size)
            && (header->requested_size == footer->requested_size));
}

// void freeBlock(void* payload_addr)
// {
//     void* header_addr = payload_addr - 8;
//     // try to coalesce
//     ics_free_header* block = coalesce((ics_free_header*) header_addr);
//     // add current block back to free list 
//     addBlockToFreeList(block);
// }

/* 
    removes the coalesced block from the freelist
*/
void removeBlock(ics_free_header* block)
{
    if (block->next != NULL)
    {
        block->next->prev = block->prev;
    }

    if (block->prev != NULL)
    {
        block->prev->next = block->next;
    }
    else
    {
        // block equal to freelist_head
        freelist_head = block->next;
    }
}

void addBlockToFreeList(ics_free_header* block)
{
    size_t block_size = getBlockSize(block);
    ics_free_header* ptr = freelist_head;
    ics_free_header* prev = NULL;
    while (ptr != NULL)
    {
        size_t next_block_size = getBlockSize(ptr);
        if (block_size <= next_block_size)
        {
            // insert block here
            break;
        }
        prev = ptr;
        ptr = ptr->next;
    }
    // insert block
    block->next = ptr;
    block->prev = prev;
    if (ptr != NULL)
    {
        // change prev pointer at next free block
        ptr->prev = block;
    }

    if (prev != NULL)
    {
        // change next pointer at previous free block
        prev->next = block;
    }
    else
    {
        // insert at front, modify freelist_head
        freelist_head = block;
    }
}

/*
    given the header to a free block (assume it is not in the free list)
    try to coalece with adjacent blocks if possible, return pointer to the 
    coalesced block
*/
ics_free_header* coalesce(ics_free_header* block)
{
    ics_header* header = (ics_header*) block;
    ics_footer* footer = getFooterAddr(header);
    size_t block_size = getBlockSize((void*)header);
    void* prev_footer_addr = ((void*)header) - 8;
    void* next_header_addr = ((void*)footer) + 8;

    if (checkAllocated(prev_footer_addr) == 0)
    {
        // coalesce with previous block
        size_t prev_block_size = getBlockSize(prev_footer_addr);
        void* prev_header_addr = getHeaderAddr(prev_footer_addr);
        header = (ics_header*) prev_header_addr;
        block_size += prev_block_size;
        removeBlock((ics_free_header*)prev_header_addr);
    }
    if (checkAllocated(next_header_addr) == 0)
    {
        // coalesce with next block
        size_t next_block_size = getBlockSize(next_header_addr);
        void* next_footer_addr = getFooterAddr(next_header_addr);
        footer = (ics_footer*) next_footer_addr;
        block_size += next_block_size;
        removeBlock((ics_free_header*)next_header_addr);
    }

    // update block size
    header->block_size = block_size;
    footer->block_size = block_size;

    ics_free_header* result = (ics_free_header*) header;
    // make it null for safety
    result->next = NULL;
    result->prev = NULL;
    return result;
}

void changeAllocatedBit(void* word, int a)
{
    ics_header* header = (ics_header*) word;
    if (a == 1)
    {
        header->block_size |= 1;
    }
    else
    {
        header->block_size &= -2;
    }
}
