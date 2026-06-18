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
 * 1. Changed on success: data[index] = *val, length may increase to index+1
 * 2. Unchanged: item_size, alloc, (current_size may change in dynamic mode)
 * 3. On failure: list remains valid (ensure_capacity failed or overflow)
 * 4. Validity invariant: aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare val: readable memory of item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable, hence readable) */

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

        /* data pointer must be non-null (we wrote something) */
        assert(list.data != NULL);

        /* The value at index matches val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

    } else {
        /* On failure: list is still valid (already asserted above) */
        /* In static mode: current_size unchanged */
        if (old.alloc == NULL) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
            assert(list.length == old.length);
        }
        /* In dynamic mode on failure (e.g., allocation failed), 
           the list should still be valid which we already checked */
    }
}
