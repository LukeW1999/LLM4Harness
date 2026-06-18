#include <aws/common/array_list.h>
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
 *   Changed on success:
 *     - list->data[index * item_size .. (index+1)*item_size) = *val  (contents at index)
 *     - list->length may increase to index+1 if index >= old length
 *     - list->current_size may increase (dynamic realloc)
 *     - list->data pointer may change (dynamic realloc)
 *   Unchanged on success:
 *     - list->item_size
 *     - list->alloc
 *   On failure:
 *     - list remains valid (ensure_capacity may have failed, but list is still valid)
 *     - returns AWS_OP_ERR
 *   Invariants:
 *     - aws_array_list_is_valid(list) always holds after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Prepare val: readable buffer of item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable for item_size bytes — CBMC treats malloc'd memory as readable */

    /* 4. Choose a non-deterministic index */
    size_t index;
    index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Invariant: list is always valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* item_size must never change */
    assert(list.item_size == old_list.item_size);

    /* alloc must never change */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length must be at least index+1 */
        assert(list.length >= index + 1);

        /* On success: if index was already within old length, length is unchanged or grew */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* index >= old length: length becomes index+1 */
            assert(list.length == index + 1);
        }

        /* On success: current_size must be large enough to hold length elements */
        assert(list.current_size >= list.length * list.item_size);

        /* On success: data must be non-NULL (we wrote something) */
        assert(list.data != NULL);

        /* On success: the value at index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure: returns AWS_OP_ERR */
        assert(result == AWS_OP_ERR);

        /* On failure: list is still valid (already asserted above) */
        /* length should not have grown beyond what it was */
        /* (ensure_capacity failed, so no write happened) */
        assert(list.length == old_list.length);

        /* item_size unchanged (already asserted) */
        /* alloc unchanged (already asserted) */

        /* current_size: in static mode it stays the same; in dynamic mode
           ensure_capacity may have failed before changing anything */
        /* We can assert current_size >= old current_size only if dynamic,
           but conservatively: current_size is still consistent with validity */
        assert(list.current_size >= list.length * list.item_size);
    }
}
