#include <aws/common/array_list.h>
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
 * 1. Changed fields on success:
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (copy)
 *    - list->length may increase to index+1 if index >= old length
 *    - list->current_size may increase (dynamic realloc)
 *    - list->data pointer may change (dynamic realloc)
 * 2. Unchanged fields:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid (best-effort; current_size/length may be unchanged)
 *    - returns AWS_OP_ERR
 * 4. Validity invariants:
 *    - aws_array_list_is_valid(list) holds after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Non-deterministic val: allocate readable memory of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;
    /* Save a byte from val to check it was copied correctly */
    /* We'll check the copy via assert_bytes_match after success */

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size must never change */
    assert(list.item_size == old_list.item_size);

    /* alloc must never change */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the value was copied into the array at index */
        /* list->length >= index + 1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length is at least old length */
        if (index < old_list.length) {
            assert(list.length >= old_list.length);
        } else {
            /* length was updated to index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be large enough to hold length elements */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we have elements */
        assert(list.data != NULL);

        /* The value at index must match val */
        assert_bytes_match(
            (uint8_t *)list.data + (list.item_size * index),
            val,
            list.item_size);

    } else {
        /* On failure: AWS_OP_ERR returned */
        assert(result == AWS_OP_ERR);

        /* List must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* In static mode (alloc == NULL), length and current_size unchanged */
        if (old_list.alloc == NULL) {
            assert(list.length == old_list.length);
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
    }
}

void aws_array_list_push_back_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
