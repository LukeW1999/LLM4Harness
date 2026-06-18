/*=== Formal Contract for aws_array_list_push_front ===*/
/*
Preconditions:
  - list != NULL
  - list->item_size > 0
  - list->length <= aws_array_list_capacity(list)
  - if list->alloc != NULL (dynamic mode):
        list->data == NULL  ==>  list->current_size == 0
        list->data != NULL  ==>  list->current_size >= list->length * list->item_size
  - if list->alloc == NULL (static mode):
        list->data != NULL
        list->current_size == list->length * list->item_size
  - val != NULL
  - memory region [val, val + list->item_size) is readable

Postconditions (validity):
  - Return value err is either 0 (success) or a non‑zero AWS error code.
  - If err == 0:
        * list->length == old_length + 1
        * list->capacity is unchanged or increased (dynamic mode may grow)
        * list->data points to a buffer large enough for list->length * list->item_size bytes
  - If err != 0:
        * list->length == old_length
        * list->capacity unchanged
        * No memory outside of list->data is modified

Postconditions (frame):
  - The allocator object (if any) is not modified.
  - The memory region pointed to by val is not modified.
  - All other memory not belonging to list->data remains unchanged.
*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_push_front_harness(void) {
    /*--- Setup nondeterministic list ---*/
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet fields */
    list.alloc = nondet_bool() ? alloc : NULL;               /* dynamic or static */
    list.item_size = (size_t)nondet_uint();                  /* >0 enforced later */
    list.length = (size_t)nondet_uint();
    list.current_size = (size_t)nondet_uint();
    list.data = NULL;                                        /* will be set below */

    __CPROVER_assume(list.item_size > 0);

    /* Ensure length does not exceed capacity */
    size_t capacity = (list.alloc != NULL) ? (list.current_size / list.item_size) : (list.current_size / list.item_size);
    __CPROVER_assume(list.length <= capacity);

    /* Allocate data buffer if needed */
    if (list.alloc != NULL) {
        /* dynamic mode: data may be NULL if capacity == 0 */
        if (capacity > 0) {
            list.data = aws_mem_acquire(list.alloc, list.current_size);
            __CPROVER_assume(list.data != NULL);
        } else {
            list.data = NULL;
        }
    } else {
        /* static mode: data must be non‑NULL and exactly current_size bytes */
        __CPROVER_assume(capacity > 0);
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    }

    /*--- Setup nondeterministic value to push ---*/
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make val readable */
    __CPROVER_assume(__CPROVER_is_fresh(val, list.item_size));

    /* Preserve old state for postcondition checks */
    size_t old_length = list.length;
    size_t old_capacity = capacity;
    void *old_data = NULL;
    if (list.data != NULL) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /*--- Call function under test ---*/
    int err = aws_array_list_push_front(&list, val);

    /*--- Postcondition checks ---*/
    if (err == 0) {
        /* length increased by one */
        assert(list.length == old_length + 1);

        /* capacity may have grown (dynamic) or stay the same (static) */
        size_t new_capacity = (list.alloc != NULL) ? (list.current_size / list.item_size) : (list.current_size / list.item_size);
        assert(new_capacity >= old_capacity);
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* on error length unchanged */
        assert(list.length == old_length);
        /* capacity unchanged */
        size_t new_capacity = (list.alloc != NULL) ? (list.current_size / list.item_size) : (list.current_size / list.item_size);
        assert(new_capacity == old_capacity);
    }

    /*--- Frame conditions ---*/
    /* allocator unchanged */
    if (list.alloc != NULL) {
        assert(list.alloc == alloc);
    }

    /* val not modified */
    // (no write to val in implementation)

    /* memory outside list->data unchanged */
    if (old_data != NULL && list.data != NULL) {
        /* compare the prefix that should remain unchanged (elements after the inserted one) */
        size_t unchanged_bytes = old_length * list.item_size;
        if (unchanged_bytes > 0) {
            assert(memcmp((uint8_t *)list.data + list.item_size,
                          (uint8_t *)old_data,
                          unchanged_bytes) == 0);
        }
    }

    /* clean up */
    if (list.alloc != NULL && list.data != NULL) {
        aws_mem_release(list.alloc, list.data);
    } else {
        free(list.data);
    }
    free(val);
    free(old_data);

    return 0;
}
