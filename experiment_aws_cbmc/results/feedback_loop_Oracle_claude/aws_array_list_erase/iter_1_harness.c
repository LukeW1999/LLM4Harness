#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 4
#define MAX_ITEM_SIZE 8

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;
    size_t index;

    /* Allocate and initialize the list with nondet data */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save pre-call state */
    size_t old_length = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    int result = aws_array_list_erase(&list, index);

    /* 1. Return value / error code correctness */
    if (index >= old_length) {
        /* Should return error when index is out of bounds */
        assert(result == AWS_OP_ERR);
        /* Length should be unchanged */
        assert(aws_array_list_length(&list) == old_length);
    } else {
        /* Should succeed when index is valid */
        assert(result == AWS_OP_SUCCESS);
        /* Length should decrease by 1 */
        assert(aws_array_list_length(&list) == old_length - 1);
    }

    /* 2. Output buffer length/capacity invariants */
    /* Capacity should never decrease (no reallocation shrinks capacity) */
    assert(aws_array_list_capacity(&list) >= aws_array_list_length(&list));

    /* item_size should never change */
    assert(list.item_size == old_item_size);

    /* current_size should not change (erase doesn't reallocate) */
    assert(list.current_size == old_current_size);

    /* 3. Memory frame conditions */
    /* The allocator should not change */
    assert(list.alloc == old_alloc);

    /* The data pointer should not change (erase works in-place) */
    assert(list.data == old_data);

    /* The list should remain valid after the operation */
    assert(aws_array_list_is_valid(&list));
}
