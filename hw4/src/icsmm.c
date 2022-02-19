#include "icsmm.h"
#include "debug.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * The allocator MUST store the head of its free list in this variable. 
 * Doing so will make it accessible via the extern keyword.
 * This will allow ics_freelist_print to access the value from a different file.
 */
ics_free_header *freelist_head = NULL;


/*
 * This is your implementation of malloc. It acquires uninitialized memory from  
 * ics_inc_brk() that is 16-byte aligned, as needed.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If successful, the pointer to a valid region of memory of at least the
 * requested size is returned. Otherwise, NULL is returned and errno is set to 
 * ENOMEM - representing failure to allocate space for the request.
 * 
 * If size is 0, then NULL is returned and errno is set to EINVAL - representing
 * an invalid request.
 */
void *ics_malloc(size_t size) { 
    if (freelist_head == NULL)
    {
        // first time calling malloc(), get a page and initialize head
        initializeHeap(&freelist_head);
    }

    // ics_freelist_print();
    // search the freelist for the first free block
    ics_free_header* nextFreeBlock = freelist_head;
    while (nextFreeBlock != NULL)
    {
        size_t blockSize = calculateRequiredBlockSize(size);
        // printf("block size: %lu\n", blockSize);
        if (nextFreeBlock->header.block_size >= blockSize)
        {
            // found a good block
            void* allocated_block = allocateBlock(size, blockSize, nextFreeBlock, &freelist_head);
            void* payload_addr = allocated_block + 8;

            printf("============after malloc()===============\n");
            ics_freelist_print();
            printf("===========================\n");
            // ics_header_print(allocated_block);
            // printf("===========================\n");
            // ics_payload_print(payload_addr);

            return payload_addr;
        }
        nextFreeBlock = nextFreeBlock->next;
    }

    // TODO: request more pages
    
    return NULL;
}

/*
 * Marks a dynamically allocated block as no longer in use and coalesces with 
 * adjacent free blocks (as specified by Homework Document). 
 * Adds the block to the appropriate bucket according to the block placement policy.
 *
 * @param ptr Address of dynamically allocated memory returned by the function
 * ics_malloc.
 * 
 * @return 0 upon success, -1 if error and set errno accordingly.
 */
int ics_free(void *ptr) { 
    if (checkBlockValid(ptr) == 0)
    {
        // invalid ptr
        errno = EINVAL;
        return -1;
    }
    ics_payload_print(ptr);
    // now free the block
    printf("==================before free()=======================\n");
    ics_freelist_print();
    printf("======================================================\n");
    freeBlock(ptr, &freelist_head);
    printf("==================after free()========================\n");
    ics_freelist_print();
    printf("======================================================\n");
    
    return 0;
}

/********************** EXTRA CREDIT ***************************************/

/*
 * Resizes the dynamically allocated memory, pointed to by ptr, to at least size 
 * bytes. See Homework Document for specific description.
 *
 * @param ptr Address of the previously allocated memory region.
 * @param size The minimum size to resize the allocated memory to.
 * @return If successful, the pointer to the block of allocated memory is
 * returned. Else, NULL is returned and errno is set appropriately.
 *
 * If there is no memory available ics_malloc will set errno to ENOMEM. 
 *
 * If ics_realloc is called with an invalid pointer, set errno to EINVAL. See ics_free
 * for more details.
 *
 * If ics_realloc is called with a valid pointer and a size of 0, the allocated     
 * block is free'd and return NULL.
 */
void *ics_realloc(void *ptr, size_t size) {
    return NULL;
}
