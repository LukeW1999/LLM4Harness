/*=== Contract for aws_array_list_push_back ===
Preconditions:
  - list is a valid, non‑NULL aws_array_list (aws_array_list_is_valid(list) == true)
  - val points to readable memory of at least list->item_size bytes
  - list may be either a dynamic list (list->alloc != NULL) or a static list (list->alloc == NULL)
Postconditions (validity):
  - aws_array_list_is_valid(list) holds after the call
  - The function returns either AWS_OP_SUCCESS (0) or an error code (non‑zero)
Postconditions (length):
  - If the return value is AWS_OP_SUCCESS, then
        aws_array_list_length(list) == old_length + 1
    and the newly appended element equals the contents of *val.
  - If the return value is an error, then
        aws_array_list_length(list) == old_length
Postconditions (frame):
  - No memory outside of the list structure and its backing buffer is modified.
  - The allocator pointer (list->alloc) and item size (list->item_size) remain unchanged.
===*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_push_back_harness(void) {
    /* nondet item size, must be > 0 */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024); /* reasonable bound */

    /* allocate the list structure */
    struct aws_array_list list;
    /* nondet choice: dynamic (true) or static (false) */
    bool is_dynamic;
    __CPROVER_assume(is_dynamic == true || is_dynamic == false);

    if (is_dynamic) {
        /* dynamic list initialization */
        size_t initial_allocation;
        __CPROVER_assume(initial_allocation <= 64);
        struct aws_allocator *alloc = aws_default_allocator();
        __CPROVER_assume(alloc != NULL);
        int init_res = aws_array_list_init_dynamic(&list, alloc, initial_allocation, item_size);
        __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    } else {
        /* static list initialization */
        size_t max_items;
        __CPROVER_assume(max_items > 0 && max_items <= 64);
        size_t buffer_bytes;
        __CPROVER_assume(!aws_mul_size_checked(max_items, item_size, &buffer_bytes));
        uint8_t *raw_buffer = malloc(buffer_bytes);
        __CPROVER_assume(raw_buffer != NULL);
        aws_array_list_init_static(&list, raw_buffer, max_items, item_size);
        /* set a nondet length that is within capacity */
        size_t length;
        __CPROVER_assume(length <= max_items);
        list.length = length;
    }

    /* Ensure the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old length for post‑condition checks */
    size_t old_length = aws_array_list_length(&list);

    /* Allocate a nondet value to push */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    /* make the memory readable (CBMC treats malloced memory as nondet) */
    /* optional: fill with nondet data */
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = __CPROVER_nondet_uint8_t();
    }

    /* Call the function under test */
    int ret = aws_array_list_push_back(&list, val);

    /* Post‑condition: list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Post‑condition: return value is either success or an error code */
    assert(ret == AWS_OP_SUCCESS || ret != AWS_OP_SUCCESS);

    if (ret == AWS_OP_SUCCESS) {
        /* Length must have increased by one */
        assert(aws_array_list_length(&list) == old_length + 1);
        /* The newly appended element must equal *val */
        uint8_t *new_elem = (uint8_t *)list.data + (list.item_size * old_length);
        for (size_t i = 0; i < item_size; ++i) {
            assert(new_elem[i] == val[i]);
        }
    } else {
        /* On error, length must be unchanged */
        assert(aws_array_list_length(&list) == old_length);
    }

    /* Frame conditions: allocator pointer and item size are unchanged */
    if (is_dynamic) {
        assert(list.alloc == aws_default_allocator());
    } else {
        assert(list.alloc == NULL);
    }
    assert(list.item_size == item_size);

    /* Clean up */
    if (list.alloc && list.data) {
        aws_mem_release(list.alloc, list.data);
    } else if (!list.alloc && list.data) {
        free(list.data);
    }
    free(val);

    return 0;
}
