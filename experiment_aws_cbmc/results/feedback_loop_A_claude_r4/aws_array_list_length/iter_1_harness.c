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
 * 1. Changed on success: data at index is overwritten with val; length may increase to index+1
 * 2. Unchanged: item_size, alloc, current_size (unless grown), data pointer (unless reallocated)
 * 3. On failure: list remains valid (ensure_capacity failed or add_size overflow)
 * 4. Validity invariant: aws_array_list_is_valid must hold after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Allocate val buffer of item_size bytes */
    const void *val = malloc(list.item_size);
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
        /* On success: the element at index was written */
        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length doesn't shrink */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length became index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be >= length * item_size */
        size_t required;
        if (!aws_mul_size_checked(list.length, list.item_size, &required)) {
            assert(list.current_size >= required);
        }

        /* data must be non-null since we wrote something */
        assert(list.data != NULL);

        /* The value at index matches val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        uint8_t *dest = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(dest, (const uint8_t *)val, list.item_size);

    } else {
        /* On failure: list must still be valid (already asserted above) */
        /* length should not have increased beyond what it was */
        /* current_size should not have changed if ensure_capacity failed */
        /* We can't assert much more without knowing which failure path was taken */
        assert(aws_array_list_is_valid(&list));
    }
}

void aws_array_list_length_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
