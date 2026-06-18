/*=== Contract ===
Preconditions:
  - aws_array_list_is_valid(list) holds.
  - val != NULL and the memory region [val, val + list->item_size) is readable.
  - index is any size_t value.

Postconditions (validity):
  - The function returns either AWS_OP_SUCCESS or AWS_OP_ERR.
  - If the return value is AWS_OP_SUCCESS then aws_array_list_is_valid(list) still holds.

Postconditions (length):
  - On success, list->length == (index >= old_length ? index + 1 : old_length).
  - In all cases, list->length <= aws_array_list_capacity(list).

Postconditions (frame):
  - list->alloc and list->item_size are unchanged.
  - If the underlying data buffer is not reallocated, then for every i:
        * if i == index, the element at position i equals the contents of val.
        * else if i < old_length, the element at position i is unchanged.
  - No memory outside the array list's data buffer is modified.
=== End Contract ===*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness(void) {
    /* Allocate and initialize a dynamic array list */
    struct aws_allocator *alloc = aws_default_allocator();
    struct aws_array_list list;
    size_t item_size;
    size_t initial_allocation;

    /* nondet values for item size and initial allocation */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024);
    __CPROVER_assume(initial_allocation <= 10);

    if (aws_array_list_init_dynamic(&list, alloc, initial_allocation, item_size) != AWS_OP_SUCCESS) {
        /* initialization failed; abort verification */
        return 0;
    }

    /* Capture old state */
    size_t old_length = list.length;
    size_t old_capacity = aws_array_list_capacity(&list);
    uint8_t *old_data = NULL;
    if (list.data != NULL) {
        old_data = malloc(old_capacity * item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_capacity * item_size);
    }

    /* nondet index */
    size_t index;
    __CPROVER_assume(index <= old_capacity * 2); /* allow index beyond current capacity */

    /* Allocate and nondet-initialize val */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    /* make val readable */
    __CPROVER_assume(__CPROVER_is_fresh(val, item_size));

    /* Call the function under test */
    int ret = aws_array_list_set_at(&list, val, index);

    /* Postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* Postcondition: validity */
    if (ret == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
    }

    /* Postcondition: length */
    if (ret == AWS_OP_SUCCESS) {
        size_t expected_length = (index >= old_length) ? index + 1 : old_length;
        assert(list.length == expected_length);
        assert(list.length <= aws_array_list_capacity(&list));
    }

    /* Frame: allocator and item size unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);

    /* Frame: data buffer contents */
    if (list.data == old_data) {
        /* Buffer was not reallocated; check element preservation */
        for (size_t i = 0; i < old_capacity; ++i) {
            uint8_t *elem_ptr = (uint8_t *)list.data + i * item_size;
            if (i == index) {
                /* Element at index must match val */
                assert(memcmp(elem_ptr, val, item_size) == 0);
            } else if (i < old_length) {
                /* Elements before old length (except index) unchanged */
                assert(memcmp(elem_ptr, old_data + i * item_size, item_size) == 0);
            }
            /* Elements beyond old_length are unspecified */
        }
    }

    /* Clean up */
    if (old_data) free(old_data);
    if (val) free(val);
    aws_array_list_clean_up(&list);
    return 0;
}
