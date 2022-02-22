#include "icsmm.h"
#include "debug.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>

#define VALUE1_VALUE 320
#define VALUE2_VALUE 0xDEADBEEFF00D

void press_to_cont() {
    printf("Press Enter to Continue");
    while (getchar() != '\n')
      ;
    printf("\n");
}

void null_check(void* ptr, long size) {
    if (ptr == NULL) {
      error(
          "Failed to allocate %lu byte(s) for an integer using ics_malloc.\n",
          size);
      error("%s\n", "Aborting...");
      assert(false);
    } else {
      success("ics_malloc returned a non-null address: %p\n", (void *)(ptr));
    }
}

void payload_check(void* ptr) {
    if ((unsigned long)(ptr) % 16 != 0) {
      warn("Returned payload address is not divisble by a quadword. %p %% 16 "
           "= %lu\n",
           (void *)(ptr), (unsigned long)(ptr) % 16);
    }
}

int main(int argc, char *argv[]) {
  // Initialize the custom allocator
  ics_mem_init();

  // Tell the user about the fields
  info("Initialized heap\n");
  press_to_cont();
  
  void *ptr0 = ics_malloc(40);
  void *ptr1 = ics_malloc(200);
  void *ptr2 = ics_malloc(300);
  void *ptr3 = ics_malloc(3000);
  // ics_free(ptr2);
  // Newly allocated blocks are tested
  ics_freelist_print();
  printf("==============================\n");
  ics_payload_print(ptr1);
  printf("==============================\n");
  ptr1 = ics_realloc(ptr1, 529);
  // Free list is printed and tested
  ics_payload_print(ptr1);
  ics_freelist_print();


  ics_mem_fini();

  return EXIT_SUCCESS;
}
