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
 * 1. Changed fields on success:
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (data written)
 *    - list->length may increase (if index >= old length, length = index + 1)
 *    - list->current_size may increase (if dynamic and capacity was insufficient)
 *    - list->data pointer may change (if dynamic realloc occurred)
 * 2. Unchanged fields:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid
 *    - returns AWS_OP_ERR
 * 4. Validity invariants:
 *    - aws_array_list_is_valid(list) holds after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Set up val: must be readable for item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable, hence readable) */

    /* 4. Choose a non-deterministic index */
    size_t index;
    index = nondet_size_t();

    /* Bound index to avoid huge state space; must be within reasonable range */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

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

        /* If index was already within old length, length is unchanged or same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length became index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient to hold the data */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we wrote something */
        assert(list.data != NULL);

        /* The value at index matches val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        /* Check bytes match at the written location */
        uint8_t *written = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(written, val, list.item_size);

    } else {
        /* On failure: list should still be valid (already asserted above) */
        /* In static mode with out-of-bounds index, list is unchanged */
        /* In dynamic mode, allocation failure may leave list partially changed,
           but validity must hold */
        assert(result == AWS_OP_ERR);

        /* item_size and alloc unchanged (already asserted) */
        /* length should not have increased beyond what was valid */
        assert(list.length <= old_list.length || list.alloc != NULL);
    }
}
