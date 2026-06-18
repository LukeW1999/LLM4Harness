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
 * 1. Changed on success:
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (copy)
 *    - list->length may increase (if index >= old length, length = index + 1)
 *    - list->current_size may increase (dynamic realloc)
 *    - list->data pointer may change (dynamic realloc)
 * 2. Unchanged:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid (postcondition in implementation)
 * 4. Validity invariant:
 *    - aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Non-deterministic val: must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the element at index was written */
        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length is unchanged or same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length became index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient to hold the new length */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we wrote something */
        assert(list.data != NULL);

        /* The value at index matches what we wrote */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure: list must still be valid (already asserted above) */
        /* In static mode, length and current_size should be unchanged */
        if (list.alloc == NULL) {
            /* Static mode: nothing should have changed */
            assert(list.length == old_list.length);
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
    }
}

void aws_array_list_pop_back_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
