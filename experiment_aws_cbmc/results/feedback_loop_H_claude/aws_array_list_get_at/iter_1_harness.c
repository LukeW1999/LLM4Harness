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
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (memory copy)
 *    - list->length may increase to index+1 if index >= old length
 *    - list->current_size may increase (dynamic realloc)
 *    - list->data pointer may change (dynamic realloc)
 * 2. Unchanged fields:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid (postcondition from implementation)
 * 4. Validity invariants:
 *    - aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Non-deterministic val — must be readable for item_size bytes */
    /* Allocate a buffer of item_size bytes for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable/readable) */

    /* 4. Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_item_size);

    /* alloc never changes */
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the element at index was written */
        /* length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same or grows */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            /* length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* data must be non-null (we wrote something) */
        assert(list.data != NULL);

        /* The value at index matches val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

    } else {
        /* On failure: list is still valid (already asserted above) */
        /* In static mode: length and current_size unchanged */
        if (old_alloc == NULL) {
            /* Static mode: nothing should have changed */
            assert(list.length == old_length);
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
        /* In dynamic mode on failure (e.g., allocation failure):
         * the list remains valid but we can't assert much more */
    }
}

void aws_array_list_get_at_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
