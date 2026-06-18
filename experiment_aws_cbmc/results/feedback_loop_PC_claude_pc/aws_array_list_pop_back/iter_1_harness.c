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
 *    - list->data contents at index*item_size are overwritten with val
 *    - list->length may increase (if index >= old length, length = index + 1)
 *    - list->current_size may increase (if dynamic and capacity was insufficient)
 *    - list->data pointer may change (if dynamic realloc occurred)
 * 2. Unchanged fields:
 *    - list->item_size (never changes)
 *    - list->alloc (never changes)
 * 3. Failure path:
 *    - Returns AWS_OP_ERR
 *    - list remains valid
 * 4. Validity invariants:
 *    - aws_array_list_is_valid(list) holds after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Non-deterministic val — must be readable for item_size bytes */
    /* Allocate a buffer for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable/readable) */

    /* 4. Save old state */
    struct aws_array_list old_list = list;
    /* Save item_size and alloc which must not change */
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* item_size must never change */
    assert(list.item_size == old_item_size);

    /* alloc must never change */
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must be valid */
        assert(aws_array_list_is_valid(&list));

        /* On success, length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            /* index >= old_length: length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient to hold length items */
        size_t required_size;
        bool no_overflow = (aws_mul_size_checked(list.length, list.item_size, &required_size) == AWS_OP_SUCCESS);
        assert(no_overflow);
        assert(list.current_size >= required_size);

        /* data must be non-null on success (we wrote something) */
        assert(list.data != NULL);

        /* The value at index must match val */
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure, length should not have increased beyond old_length
         * (the function returns early on ensure_capacity failure) */
        assert(list.length == old_length);

        /* On failure, current_size should not have changed
         * (ensure_capacity failed, so no realloc succeeded) */
        assert(list.current_size == old_current_size);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
