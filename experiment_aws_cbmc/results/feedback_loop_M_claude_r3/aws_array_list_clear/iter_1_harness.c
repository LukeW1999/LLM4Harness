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
 * 1. Changed on success:
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (memcpy)
 *    - list->length may increase to index+1 if index >= old length
 * 2. Unchanged:
 *    - list->item_size, list->alloc (not modified by set_at itself)
 *    - list->current_size may change (via ensure_capacity in dynamic mode)
 * 3. On failure: list remains valid (postcondition in implementation)
 * 4. Validity invariant: aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up the index — must be bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Set up val — must be readable for item_size bytes */
    /* item_size is already bounded by MAX_ITEM_SIZE via aws_array_list_is_bounded */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable, hence readable) */

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
        /* On success: length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length is unchanged or same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* index >= old length: length becomes index+1 */
            assert(list.length == index + 1);
        }

        /* data pointer must be non-null (we wrote to it) */
        assert(list.data != NULL);

        /* The value at index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

        /* current_size must be large enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);

    } else {
        /* On failure: list remains valid (already asserted above) */
        /* In static mode, length and current_size should be unchanged */
        if (list.alloc == NULL) {
            /* Static mode: no reallocation possible */
            assert(list.length == old_list.length);
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
        /* In dynamic mode, failure could be due to allocation failure;
           the list should still be valid (asserted above) */
    }
}
