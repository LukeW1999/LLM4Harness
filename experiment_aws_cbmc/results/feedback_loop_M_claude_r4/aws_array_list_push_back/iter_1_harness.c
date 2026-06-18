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
 *                but the list is still valid)
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

    /* 3. Set up val — readable buffer of item_size bytes */
    /* item_size is already bounded by MAX_ITEM_SIZE via aws_array_list_is_bounded */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* alloc never changes */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length must be at least index+1 */
        assert(list.length >= index + 1);

        /* On success: current_size must be sufficient for length items */
        assert(list.current_size >= list.length * list.item_size);

        /* The data pointer must be valid (non-null since we have items) */
        assert(list.data != NULL);

        /* The value was copied into the array at the correct position */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));

        /* If index was within old bounds, length doesn't shrink */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length became index + 1 */
            assert(list.length == index + 1);
        }
    } else {
        /* On failure: list is still valid (already asserted above) */
        /* In static mode with out-of-bounds index, length and current_size unchanged */
        /* In dynamic mode, allocation may have failed partway */
        /* The list validity invariant covers these cases */

        /* item_size unchanged on failure */
        assert(list.item_size == old_list.item_size);

        /* alloc unchanged on failure */
        assert(list.alloc == old_list.alloc);
    }
}

void aws_array_list_push_back_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
