#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INITIAL_ITEM_ALLOCATION 8U
#define MAX_ITEM_SIZE               32U

void aws_array_list_pop_front_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();
    struct aws_array_list list;

    /* Initialize a list with bounded capacity and item size */
    aws_array_list_init(&list, alloc, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    /* Ensure the internal buffer is allocated */
    __CPROVER_assume(list.data != NULL);

    /* Nondeterministically set a valid length */
    size_t max_len = list.current_size / list.item_size;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);
    list.length = len;

    /* Preconditions (list is now valid and bounded) */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot of pre‑state */
    size_t old_length   = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t item_size    = list.item_size;

    uint8_t *old_data = NULL;
    if (list.data != NULL && old_capacity > 0 && item_size > 0) {
        /* bound the copy size to avoid overflow in CBMC */
        __CPROVER_assume(old_capacity <= MAX_INITIAL_ITEM_ALLOCATION);
        __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
        __CPROVER_assume(old_capacity * item_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);

        old_data = malloc(old_capacity * item_size);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_assume(old_data != list.data);          /* avoid overlap */
        memcpy(old_data, list.data, old_capacity * item_size);
    }

    /* Call under verification */
    int rv = aws_array_list_pop_front(&list);

    /* Post‑conditions */

    /* 1. Return value / error code correctness */
    if (old_length > 0) {
        assert(rv == AWS_OP_SUCCESS);
    } else {
        assert(rv == AWS_ERROR_LIST_EMPTY);
    }

    /* 2. Length / capacity invariants */
    if (old_length > 0) {
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(aws_array_list_length(&list) == old_length);
    }
    assert(list.item_size == item_size);
    assert(aws_array_list_capacity(&list) == old_capacity);
    assert(list.alloc == alloc); /* allocator unchanged */

    /* 3. Frame conditions on the data buffer */
    if (list.data != NULL && old_data != NULL && item_size > 0) {
        size_t new_length = aws_array_list_length(&list);
        for (size_t i = 0; i < new_length; ++i) {
            /* element i after pop_front must equal former element i+1 */
            assert(
                memcmp(
                    old_data + (i + 1) * item_size,
                    (uint8_t *)list.data + i * item_size,
                    item_size) == 0);
        }
    }

    return;
}
