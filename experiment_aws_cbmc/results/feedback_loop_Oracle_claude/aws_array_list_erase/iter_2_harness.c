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

    /* Use a concrete allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the list with bounded, valid state */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialize a dynamic list */
    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Push some nondet number of items to give the list a nondet length */
    size_t num_items;
    __CPROVER_assume(num_items <= MAX_INITIAL_ITEM_ALLOCATION);

    for (size_t i = 0; i < num_items; i++) {
        uint8_t item[MAX_ITEM_SIZE];
        aws_array_list_push_back(&list, item);
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save pre-call state */
    size_t old_length = aws_array_list_length(&list);
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
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

    /* 2. Capacity invariant */
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

    /* Cleanup */
    aws_array_list_clean_up(&list);
}
