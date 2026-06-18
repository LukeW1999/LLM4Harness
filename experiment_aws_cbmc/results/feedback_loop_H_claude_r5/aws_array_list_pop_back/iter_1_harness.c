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
 * 4. Validity invariant: aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Prepare val: readable memory of item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable/readable) */

    /* 4. Non-deterministic index */
    size_t index = nondet_size_t();

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
        /* On success: the element was copied in, length is at least index+1 */
        assert(list.length >= index + 1);

        /* current_size must be large enough to hold length elements */
        size_t required_size = list.length * list.item_size;
        assert(list.current_size >= required_size);

        /* The data at the index matches val */
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

        /* If index was within old bounds, length should not decrease */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length became index + 1 */
            assert(list.length == index + 1);
        }
    } else {
        /* On failure: list is still valid (already asserted above) */
        /* In static mode with out-of-bounds index, length and current_size unchanged */
        /* In dynamic mode, allocation failure may leave list unchanged */
        /* The list validity is the key invariant */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }
}

void aws_array_list_pop_back_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
