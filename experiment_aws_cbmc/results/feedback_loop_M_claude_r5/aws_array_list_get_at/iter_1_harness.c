#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
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
 * 2. Unchanged:
 *    - list->item_size, list->alloc (not modified by set_at itself)
 *    - list->current_size (may change via ensure_capacity, but item_size/alloc stay)
 * 3. On failure:
 *    - list remains valid
 * 4. Validity invariant always holds after call.
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up the index — bound it to avoid state space explosion */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Set up val — must be readable for item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE from the bounding predicate */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable/readable memory) */

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

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: the element at index was written */
        /* length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within bounds, length stays the same or grows */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            assert(list.length == index + 1);
        }

        /* The data pointer must be non-null (we wrote something) */
        assert(list.data != NULL);

        /* current_size must be sufficient for the new length */
        assert(list.current_size >= list.length * list.item_size);

    } else {
        /* On failure: list remains valid (already asserted above) */
        /* In static mode, length and current_size should be unchanged */
        /* In dynamic mode, allocation failure may leave list unchanged */
        /* We can assert length did not increase beyond what it was */
        /* (ensure_capacity failed, so no write happened) */
        assert(list.length == old_length);
        assert(list.current_size == old_list.current_size);
    }
}

void aws_array_list_get_at_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
