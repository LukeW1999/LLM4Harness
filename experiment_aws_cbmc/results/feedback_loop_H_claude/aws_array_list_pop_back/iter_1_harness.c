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
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (memory copy)
 *    - list->length may increase to index+1 if index >= old length
 *    - list->current_size may increase (dynamic mode realloc)
 *    - list->data pointer may change (dynamic mode realloc)
 * 2. Unchanged fields:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure: list remains valid (ensure_capacity may fail)
 * 4. Validity invariant: aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Allocate val with item_size bytes readable */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* alloc never changes */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length is unchanged or same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length updated to index+1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient to hold the data */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we have elements */
        assert(list.data != NULL);

        /* The value at index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

    } else {
        /* On failure, list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure, length should not have increased beyond old length */
        /* (ensure_capacity failed, so no write happened) */
        assert(list.length == old_list.length);

        /* current_size unchanged on failure */
        assert(list.current_size == old_list.current_size);
    }
}

void aws_array_list_pop_back_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
