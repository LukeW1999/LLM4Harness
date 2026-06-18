#include <aws/common/array_list.h>
#include <aws/common/math.h>
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
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (memcpy)
 *    - list->length may increase to index+1 if index >= old length
 *    - list->current_size may increase (dynamic mode, ensure_capacity)
 * 2. Unchanged:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid (postcondition in implementation)
 * 4. Validity invariant:
 *    - aws_array_list_is_valid(list) always holds after call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up the index — bound it to prevent state space explosion */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Set up val — must be readable for item_size bytes */
    /* Allocate a buffer of item_size bytes for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable/readable memory) */

    /* 4. Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

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
        /* On success: the element at index was set */
        /* length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length is unchanged or same */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            /* index >= old_length: length becomes index+1 */
            assert(list.length == index + 1);
        }

        /* current_size must be >= length * item_size */
        size_t required_size = 0;
        /* We can't call aws_mul_size_checked easily here, just check the invariant via is_valid */
        assert(list.current_size >= list.length * list.item_size);

        /* The data at the index position matches val */
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure: list is still valid (already asserted above) */
        /* In static mode with out-of-bounds index, list is unchanged */
        /* length should not have increased beyond what it was */
        /* The list validity is the main postcondition */
        assert(aws_array_list_is_valid(&list));
    }
}
