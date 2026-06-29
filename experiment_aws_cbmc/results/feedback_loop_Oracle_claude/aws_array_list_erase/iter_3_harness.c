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

    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    if (init_result != AWS_OP_SUCCESS) {
        return;
    }

    /* Push a nondet number of items */
    size_t num_items;
    __CPROVER_assume(num_items <= MAX_INITIAL_ITEM_ALLOCATION);

    for (size_t i = 0; i < num_items; i++) {
        uint8_t item[MAX_ITEM_SIZE];
        /* item_size <= MAX_ITEM_SIZE, so this is safe */
        aws_array_list_push_back(&list, (void *)item);
    }

    /* Ensure the list is valid before proceeding */
    if (!aws_array_list_is_valid(&list)) {
        aws_array_list_clean_up(&list);
        return;
    }

    /* Save pre-call state */
    size_t old_length = aws_array_list_length(&list);
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* index is fully nondet - no assume on it */
    /* Call the function under test */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions */
    if (index >= old_length) {
        assert(result == AWS_OP_ERR);
        assert(aws_array_list_length(&list) == old_length);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == old_length - 1);
    }

    /* item_size should never change */
    assert(list.item_size == old_item_size);

    /* current_size should not change */
    assert(list.current_size == old_current_size);

    /* allocator should not change */
    assert(list.alloc == old_alloc);

    /* data pointer should not change */
    assert(list.data == old_data);

    /* list should remain valid */
    assert(aws_array_list_is_valid(&list));

    /* capacity >= length */
    assert(aws_array_list_capacity(&list) >= aws_array_list_length(&list));

    aws_array_list_clean_up(&list);
}
