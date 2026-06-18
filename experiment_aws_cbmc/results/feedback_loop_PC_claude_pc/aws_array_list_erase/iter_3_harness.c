#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_erase_harness() {
    struct aws_array_list list;

    /* Set up item_size with small bound */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Set up number of items */
    size_t num_items;
    __CPROVER_assume(num_items <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Compute current_size safely */
    size_t current_size;
    if (num_items == 0) {
        current_size = 0;
    } else {
        /* Avoid overflow */
        __CPROVER_assume(item_size <= SIZE_MAX / num_items);
        current_size = item_size * num_items;
    }

    /* Set up list fields */
    list.alloc = aws_default_allocator();
    list.item_size = item_size;
    list.current_size = current_size;

    if (current_size > 0) {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* length must be <= num_items */
    size_t length = nondet_size_t();
    __CPROVER_assume(length <= num_items);
    list.length = length;

    /* Ensure the list is valid before calling */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic index */
    size_t index = nondet_size_t();

    /* Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Save a byte from the buffer for data-unchanged checks */
    struct store_byte_from_buffer old_byte;
    bool saved_byte = false;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
        saved_byte = true;
    }

    /* Call the function under test */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions: structural fields never change */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    if (result == AWS_OP_SUCCESS) {
        /* On success, index must have been in bounds */
        assert(index < old_length);
        /* Length decreases by exactly 1 */
        assert(list.length == old_length - 1);
    } else {
        /* On failure, index was out of bounds */
        assert(index >= old_length);
        /* List length is unchanged */
        assert(list.length == old_length);
        /* Data contents unchanged */
        if (saved_byte && list.current_size > 0 && list.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* List validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
