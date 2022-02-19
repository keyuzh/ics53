#include "helpers.h"
#include "debug.h"

// #include "icsmm.h"

/* Helper function definitions go here */

size_t getBlockSize(void* word)
{
    return ((ics_header*)word)->block_size & -2;
}

// helpers for malloc()

void initializeHeap(ics_free_header** freelist_head)
{
    // printf("getting first page\n");
    // get a page of heap
    void* start = ics_inc_brk();
    void* end = ics_get_brk();
    // printf("start: %p\nend: %p\n", start, end);

    // create a prologue and epilogue
    ics_footer* prologue = (ics_footer*) start;
    ics_header* epilogue = (ics_header*) (end - 8);
    // printf("prologue: %p\n", prologue);
    // printf("epilogue: %p\n", epilogue);
    *prologue = (ics_footer) {1, FOOTER_MAGIC, 0};
    *epilogue = (ics_header) {1, HEADER_MAGIC, 0};

    // ics_header_print(prologue);
    // ics_header_print(epilogue);

    // make a big free block
    ics_header* head = (ics_header*) (start + 8);
    ics_footer* foot = (ics_footer*) (end - 16);
    // size of big free block = 4096 - 8 (prologue) - 8 (epilogue);
    *head = (ics_header) {4080, HEADER_MAGIC, 0};
    *foot = (ics_footer) {4080, FOOTER_MAGIC, 0};

    // ics_free_header* ics_free_next = (ics_free_header*) (start + 16);
    // ics_free_header* ics_free_prev = (ics_free_header*) (start + 24);
    // ics_free_next = NULL;
    // ics_free_prev = NULL;

    // let freelist_head point to the big free block
    *freelist_head = (ics_free_header*) head;
    (*freelist_head)->next = NULL;
    (*freelist_head)->prev = NULL;

    // ics_header_print(*freelist_head);
}

/*
    given the size of malloc call, calculate the block size that is required
    on the heap
*/
size_t calculateRequiredBlockSize(size_t size)
{
    // payload size = request size + (16 - (request size %16))
    // total block size = payload size + 8(header) + 8(footer)
    return 8 + 8 + (size + (16 - (size % 16)));
}

/*
    allocate the block, modify head pointer if needed
*/
void* allocateBlock(size_t requested_size, size_t block_size, ics_free_header* block, ics_free_header** freelist_head)
{
    // bytes remaining in the free block after allocating
    size_t remaining_size = block->header.block_size - block_size;
    if (remaining_size < 32)
    {
        // if the remaining block is too small, add it to this block
        block_size += remaining_size;
    }
    else
    {
        // move the header to the splited block
        void* header_location_after_split = (void*)block;
        header_location_after_split += block_size;
        ics_free_header* new_header = (ics_free_header*) header_location_after_split;
        new_header->header = (ics_header) {remaining_size, HEADER_MAGIC, 0};
        // update block on footer 
        void* footer_location = (void*)block;
        footer_location += block->header.block_size - 8;
        ics_footer* footer = (ics_footer*) footer_location;
        footer->block_size = remaining_size;
        // move the prev and next pointer
        new_header->next = block->next;
        new_header->prev = block->prev;
        // if prev pointer is null, it means that the block is the first block
        // of the explicit list, move the freelist_head pointer as well
        if (new_header->prev == NULL)
        {
            *freelist_head = new_header;
        }
    }
    // create header and footer for the allocated block
    ics_header* header = (ics_header*)block;
    header->block_size = block_size | 1;
    header->requested_size = requested_size;
    void* footer_location = (void*) block;
    footer_location += block_size - 8;
    ics_footer* footer = (ics_footer*) footer_location;
    *footer = (ics_footer) {block_size | 1, FOOTER_MAGIC, requested_size};

    // return the address of the new block
    return (void*)header;
}



// helpers for free()

int checkBlockValid(void* payload_addr)
{
    void* header_addr = payload_addr - 8;
    // TODO: check header_addr is valid (no segfault)
    ics_header* header = (ics_header*) header_addr;
    void* footer_addr = header_addr + (header->block_size & -2) - 8;
    ics_footer* footer = (ics_footer*) footer_addr;

    // int result = 1;
    // check the following
    // ptr between prologue and epilogue

    // allocated bit 'a' is set in both header and footer
    return (
    checkAllocated((void*)header)
    && checkAllocated((void*)footer)
    // hid field of header
    && checkHeaderField(header)
    // fid field of footer
    && checkFootererField(footer)
    // block size in header and footer are equal
    // requested size in header and footer are equal
    && checkSizeEqual(header, footer)
    );
}

int checkAllocated(void* word)
{
    return (((ics_header*)word)->block_size & 1);
}

// int checkBlockIsAllocated(ics_header* header, ics_footer* footer)
// {
//     int header_a = header->block_size & 1;
//     int footer_a = footer->block_size & 1;
//     return (header_a && footer_a);
// }

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

void freeBlock(void* payload_addr, ics_free_header** freelist_head)
{
    void* header_addr = payload_addr - 8;
    ics_header* header = (ics_header*) header_addr;
    void* footer_addr = header_addr + (header->block_size & -2) - 8;
    ics_footer* footer = (ics_footer*) footer_addr;

    size_t block_size = (header->block_size & -2);
    
    // coalescing
    void* prev_footer_addr = header_addr - 8;
    // ics_footer* prev_footer = (ics_footer*) prev_footer_addr;
    void* next_header_addr = footer_addr + 8;
    // ics_header* next_header = (ics_header*) next_header_addr;
    if (checkAllocated(prev_footer_addr) == 0)
    {
        // coalesce with previous block
        size_t prev_block_size = ((ics_footer*)prev_footer_addr)->block_size;
        void* prev_header_addr = prev_footer_addr + 8 - prev_block_size;
        header = (ics_header*) prev_header_addr;
        block_size += prev_block_size;
        removeBlock((ics_free_header*)prev_header_addr, freelist_head);
    }
    if (checkAllocated(next_header_addr) == 0)
    {
        // coalesce with next block
        size_t next_block_size = getBlockSize(next_header_addr);
        void* next_footer_addr = next_header_addr + next_block_size - 8;
        footer = (ics_footer*) next_footer_addr;
        block_size += next_block_size;
        removeBlock((ics_free_header*)next_header_addr, freelist_head);
    }

    // make the header and footer "free"
    // *header = (ics_header){block_size, HEADER_MAGIC, 0};
    header->block_size = block_size;
    // *footer = (ics_footer){block_size, FOOTER_MAGIC, 0};
    footer->block_size = block_size;

    // add current block back to free list 
    addBlockToFreeList((ics_free_header*)header, freelist_head);
    
    return;

}

/* 
    removes the coalesced block from the freelist
*/
void removeBlock(ics_free_header* block, ics_free_header** freelist_head)
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
        // block must equal to freelist_head
        *freelist_head = block->next;
    }
}

void addBlockToFreeList(ics_free_header* block, ics_free_header** freelist_head)
{
    size_t block_size = block->header.block_size;
    ics_free_header* ptr = *freelist_head;
    ics_free_header* prev = NULL;
    while (ptr != NULL)
    {
        size_t next_block_size = ptr->header.block_size;
        if (block_size <= next_block_size)
        {
            // insert block here
            break;
        }
        prev = ptr;
        ptr = ptr->next;
    }

    block->next = ptr;
    block->prev = prev;
    if (ptr != NULL)
    {
        ptr->prev = block;
    }

    if (prev != NULL)
    {
        prev->next = block;
    }
    else
    {
        // insert at front, modify freelist_head
        *freelist_head = block;
    }
}
