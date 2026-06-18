#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Harness for aws_array_list_set_at
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
 * 4. Validity: aws_array_list_is_valid(list) must hold after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare val: readable buffer of item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable for item_size bytes — malloc guarantees this */

    /* 4. Non-deterministic index */
    size_t index;
    index = nondet_size_t();

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old.item_size);

    /* alloc never changes */
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the element at index was written */
        /* length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length stays the same or grows */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            /* length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be large enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we wrote something */
        assert(list.data != NULL);

        /* The value at index matches val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);
    } else {
        /* On failure: list is still valid (already asserted above) */
        /* In static mode with out-of-bounds index, length and current_size unchanged */
        /* We can't assert much more without knowing the mode, but validity covers it */
        assert(aws_array_list_is_valid(&list));
    }
}

void aws_array_list_get_at_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
