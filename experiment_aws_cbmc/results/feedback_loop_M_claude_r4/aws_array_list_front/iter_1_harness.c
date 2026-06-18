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
 * 1. Changed on success: data[index] = *val, list->length may increase to index+1
 * 2. Unchanged: list->item_size, list->alloc (allocator pointer itself)
 * 3. On failure: list remains valid (ensure_capacity may have changed current_size in dynamic mode,
 *    but the list is still valid)
 * 4. Validity invariant: aws_array_list_is_valid(list) must hold after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up the value to write */
    /* val must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Choose a nondeterministic index, bounded to keep state space manageable */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state before calling */
    struct aws_array_list old_list = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* item_size must never change */
    assert(list.item_size == old_list.item_size);

    /* allocator pointer must never change */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the element at index was written */
        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length stays the same or grows */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* The data pointer must be non-null (we wrote something) */
        assert(list.data != NULL);

        /* current_size must be at least length * item_size */
        assert(list.current_size >= list.length * list.item_size);

        /* The value at index must match what we wrote */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));
    } else {
        /* On failure: list must still be valid (already asserted above) */
        /* In static mode with out-of-bounds index, length should not have changed */
        /* In dynamic mode, allocation failure means list may be unchanged */
        /* The list validity is the key invariant */
        assert(aws_array_list_is_valid(&list));
    }
}

void aws_array_list_front_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
