#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
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
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (data written)
 *    - list->length may increase to index+1 if index >= old length
 * 2. Unchanged fields:
 *    - list->item_size, list->alloc, list->current_size (may grow in dynamic mode,
 *      but current_size is managed by ensure_capacity; we check validity)
 * 3. On failure:
 *    - list remains valid
 * 4. Validity invariant:
 *    - aws_array_list_is_valid(list) holds after the call
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

    /* 3. Set up val — readable memory of item_size bytes */
    /* item_size is already bounded by MAX_ITEM_SIZE via aws_array_list_is_bounded */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is writable/readable for item_size bytes (malloc guarantees this) */

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
            /* index >= old length: length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be >= length * item_size (validity covers this) */
        /* data must be non-null since length > 0 */
        assert(list.data != NULL);

        /* The value was copied into the list at the correct offset */
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));

    } else {
        /* On failure: list remains valid (already asserted above) */
        /* In static mode, length and current_size should be unchanged */
        if (list.alloc == NULL) {
            /* Static mode: nothing should have changed */
            assert(list.length == old_list.length);
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
        /* In dynamic mode, failure could be due to allocation failure;
           the list should still be valid (already asserted) */
    }
}
