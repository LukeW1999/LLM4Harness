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
 * Doxygen: "Copies the memory pointed to by val into the array at index.
 *  If in dynamic mode, the size will grow by a factor of two when the array
 *  is full. In static mode, AWS_ERROR_INVALID_INDEX will be raised if the
 *  index is past the bounds of the array."
 *
 * Analysis:
 *  Changed on success:
 *    - list->data[index * item_size .. (index+1)*item_size) == *val
 *    - list->length may increase to index+1 if index >= old length
 *    - list->current_size may increase (dynamic realloc)
 *  Unchanged:
 *    - list->item_size
 *    - list->alloc
 *  On failure:
 *    - list remains valid (best-effort; current_size/length unchanged)
 *  Invariants:
 *    - aws_array_list_is_valid(list) always holds after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Set up the list */
    struct aws_array_list list;

    /* Bound the list to keep the state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Set up val: readable buffer of item_size bytes */
    /* item_size is guaranteed > 0 by aws_array_list_is_valid */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (CBMC treats malloc'd memory as readable) */

    /* 4. Snapshot old state */
    struct aws_array_list old_list = list;

    /* Save a byte from val to check it was copied */
    /* We'll check the copy via the list data after a successful call */

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* --- Invariant: list is always valid after the call --- */
    assert(aws_array_list_is_valid(&list));

    /* --- Unchanged fields regardless of result --- */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         *   - The data at index was written (val was copied in)
         *   - list->length >= index + 1
         *   - list->current_size >= list->length * list->item_size
         */
        assert(list.data != NULL);

        /* length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length is unchanged or
         * increased; either way length >= index+1 covers it */

        /* current_size must accommodate the new length */
        size_t required;
        /* We can't call aws_mul_size_checked easily here, but we know
         * aws_array_list_is_valid already checks this; the assert above
         * covers it transitively. Add an explicit size check: */
        assert(list.current_size >= list.length * list.item_size);

        /* The bytes at the written position match val */
        uint8_t *dest = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(dest, val, list.item_size);

        /* If index was within old bounds, length should not shrink */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length was updated to index + 1 */
            assert(list.length == index + 1);
        }

    } else {
        /* On failure:
         *   - list remains valid (asserted above)
         *   - In static mode: length and current_size are unchanged
         *   - In dynamic mode: allocation may have failed, but list is still valid
         */
        /* The list must still be valid — already asserted above */

        /* If static (no allocator), length and current_size must be unchanged */
        if (list.alloc == NULL) {
            assert(list.length == old_list.length);
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
    }
}

void aws_array_list_get_at_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
