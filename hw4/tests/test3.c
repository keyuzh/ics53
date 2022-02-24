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
  
  
  void *ptr0 = ics_malloc(413);// free 432 -> ptr13
void *ptr1 = ics_malloc(80);
void *ptr2 = ics_malloc(91);
void *ptr3 = ics_malloc(574); // after last free() 592  ptr17(544)
 // ptr15() ptr16 ptr18 ptr20 free 304
void *ptr4 = ics_malloc(346);
void *ptr5 = ics_malloc(817);
void *ptr6 = ics_malloc(326);// free 352 ptr15(80) ptr16(48) ptr18(112)
void *ptr7 = ics_malloc(5000);
void *ptr8 = ics_malloc(32);// free     ptr14
void *ptr9 = ics_malloc(385);// free    
void *ptr10 = ics_malloc(236);// free    ptr17
void *ptr11 = ics_malloc(571);// free    ptr19
void *ptr12 = ics_malloc(1419);// free
ics_free(ptr3);
ics_free(ptr10);
ics_free(ptr0);
ics_free(ptr11);
ics_free(ptr12);
ics_free(ptr9);
ics_free(ptr6);
ics_free(ptr8);
void *ptr13 = ics_malloc(413);
void *ptr14 = ics_malloc(795);
void *ptr15 = ics_malloc(63);
void *ptr16 = ics_malloc(24);
void *ptr17 = ics_malloc(523);
void *ptr18 = ics_malloc(86);
void *ptr19 = ics_malloc(448);
void *ptr20 = ics_malloc(32);
void *ptr21 = ics_malloc(75);

  ics_freelist_print();


  ics_mem_fini();

  return EXIT_SUCCESS;
}
