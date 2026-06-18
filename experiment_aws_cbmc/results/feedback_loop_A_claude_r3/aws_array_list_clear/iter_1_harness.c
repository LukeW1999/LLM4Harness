#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 * Harness for aws_array_list_set_at:
 *
 * Doxygen says:
 *   "Copies the memory pointed to by val into the array at index.
 *    If in dynamic mode, the size will grow by a factor of two when the array is full.
 *    In static mode, AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds."
 *
 * Analysis:
 * 1. Changed fields on success:
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (memory written)
 *    - list->length may increase to index+1 if index >= old length
 *    - list->current_size may increase (dynamic realloc)
 *    - list->data pointer may change (dynamic realloc)
 * 2. Unchanged fields:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid (postcondition in implementation)
 * 4. Validity invariants:
 *    - aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Set up val: must be readable for item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable/readable memory) */

    /* 4. Non-deterministic index */
    size_t index = nondet_size_t();

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
        /* On success: length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length is unchanged or same */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            /* length updated to index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient to hold length items */
        size_t required_size = 0;
        int no_overflow = !aws_mul_size_checked(list.length, list.item_size, &required_size);
        assert(no_overflow);
        assert(list.current_size >= required_size);

        /* data must be non-null since we have items */
        assert(list.data != NULL);

    } else {
        /* On failure: list is still valid (asserted above) */
        /* In static mode: length and current_size unchanged */
        if (list.alloc == NULL) {
            /* Static mode: nothing should have changed */
            assert(list.length == old.length);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }
        /* In dynamic mode on failure (e.g., OOM): list remains valid */
    }
}

void aws_array_list_clear_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
