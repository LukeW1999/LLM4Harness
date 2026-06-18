#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Harness for aws_array_list_set_at:
 *
 * Doxygen says:
 *   "Copies the memory pointed to by val into the array at index.
 *    If in dynamic mode, the size will grow by a factor of two when the array is full.
 *    In static mode, AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds."
 *
 * Analysis:
 * 1. Changed on success: data[index*item_size .. (index+1)*item_size) = *val
 *                        list->length = max(old_length, index+1)
 *                        list->current_size may grow (dynamic mode)
 * 2. Unchanged: list->item_size, list->alloc
 * 3. On failure: list remains valid (ensure_capacity may have changed current_size/data
 *                but the function returns AWS_OP_ERR)
 * 4. Validity invariant: aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up the index — bound it to prevent state space explosion */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Set up val — must be readable for item_size bytes */
    /* item_size is already bounded by MAX_ITEM_SIZE via aws_array_list_is_bounded */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* item_size must never change */
    assert(list.item_size == old_item_size);

    /* allocator must never change */
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the element at index was written */
        /* length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within bounds, length is max(old_length, index+1) */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* The data pointer must be valid (non-null since we wrote something) */
        assert(list.data != NULL);

        /* current_size must be large enough to hold length items */
        assert(list.current_size >= list.length * list.item_size);

        /* The value at index must match val */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));
    } else {
        /* On failure: list must still be valid */
        /* In static mode with out-of-bounds index, list is unchanged */
        /* In dynamic mode, allocation failure may leave list partially changed,
         * but validity must hold */
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
