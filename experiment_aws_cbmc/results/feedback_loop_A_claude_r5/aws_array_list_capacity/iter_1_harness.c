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
 * 1. Changed on success: data at index is overwritten with *val; if index >=
 *    old length, list->length becomes index+1; current_size may grow (dynamic).
 * 2. Unchanged: item_size, alloc pointer.
 * 3. On failure: list remains valid (postcondition in implementation).
 * 4. Validity invariant: aws_array_list_is_valid must hold after the call.
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare val: a readable buffer of item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable for item_size bytes — malloc guarantees this */

    /* 4. Choose a non-deterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old.item_size);

    /* alloc pointer never changes */
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the data was written at index */
        /* If index was >= old length, length becomes index + 1 */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            /* length stays the same if index was within bounds */
            assert(list.length == old.length);
        }

        /* current_size must be at least enough to hold the new length */
        size_t required_size = list.length * list.item_size;
        assert(list.current_size >= required_size);

        /* The data pointer must be non-null (we wrote something) */
        assert(list.data != NULL);

        /* The value at index matches what we wrote */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        /* Check bytes match at the written position */
        uint8_t *dest = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(dest, val, list.item_size);

    } else {
        /* On failure, the list must still be valid (already asserted above) */
        /* In static mode, length should not have changed */
        /* In dynamic mode, allocation may have failed leaving list unchanged */
        /* The list validity is the key invariant */

        /* current_size should not have shrunk */
        assert(list.current_size >= old.current_size ||
               list.current_size == old.current_size);

        /* item_size unchanged (already asserted) */
        /* alloc unchanged (already asserted) */
    }
}
