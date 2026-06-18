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
 * From Doxygen:
 * "Copies the memory pointed to by val into the array at index. If in dynamic
 *  mode, the size will grow by a factor of two when the array is full. In
 *  static mode, AWS_ERROR_INVALID_INDEX will be raised if the index is past
 *  the bounds of the array."
 *
 * Analysis:
 * 1. Changed on success: data[index*item_size .. (index+1)*item_size) = *val;
 *    if index >= old length, length = index + 1.
 *    current_size may grow (dynamic mode).
 * 2. Unchanged: item_size, alloc pointer.
 * 3. On failure: list is still valid; length/current_size unchanged.
 * 4. Validity invariant: aws_array_list_is_valid(list) always holds after call.
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

    /* alloc pointer never changes */
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the value was copied into the array at index */
        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old length, length is unchanged or grew */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            /* length = index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient to hold the new length */
        size_t required_size;
        bool no_overflow = (aws_mul_size_checked(list.length, list.item_size, &required_size) == AWS_OP_SUCCESS);
        assert(no_overflow);
        assert(list.current_size >= required_size);

        /* The data pointer must be non-null (we wrote something) */
        assert(list.data != NULL);

        /* The value at index matches val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

    } else {
        /* On failure: list remains valid (already asserted above) */
        /* length and current_size should be unchanged on failure */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }
}
