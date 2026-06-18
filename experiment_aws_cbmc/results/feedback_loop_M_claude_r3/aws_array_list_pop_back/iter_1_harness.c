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
 * 2. Unchanged: item_size, alloc, current_size (unless grown — but we use static for simplicity)
 * 3. On failure: list remains valid, error is raised
 * 4. Validity invariant: aws_array_list_is_valid(list) holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up the value to write — must be readable for item_size bytes */
    size_t item_size = list.item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable, hence readable) */

    /* 3. Choose a nondet index, bounded to avoid huge state space */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* alloc never changes */
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

        /* current_size must be >= length * item_size */
        size_t required_size = list.length * list.item_size;
        assert(list.current_size >= required_size);

        /* The data pointer must be non-null since we wrote something */
        assert(list.data != NULL);

        /* The value at index matches what we wrote */
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, item_size);

    } else {
        /* On failure: list must still be valid (already asserted above) */
        /* In static mode with out-of-bounds index, list is unchanged */
        /* current_size unchanged */
        assert(list.current_size == old_list.current_size);
        /* length unchanged */
        assert(list.length == old_list.length);
        /* data pointer unchanged */
        assert(list.data == old_list.data);
    }
}
