#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Harness for aws_array_list_set_at:
 * Copies the memory pointed to by val into the array at index.
 * In dynamic mode, the size will grow by a factor of two when the array is full.
 * In static mode, AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds.
 */
void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Bound and create the value to set */
    /* item_size is already bounded by MAX_ITEM_SIZE via aws_array_list_is_bounded */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Bound the index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must not change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The data was copied into the array at index
         * - If index >= old length, length becomes index + 1
         * - current_size may have grown (dynamic mode)
         */
        assert(list.data != NULL);

        /* Length update: if index was >= old length, new length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* index was within bounds, length unchanged */
            assert(list.length == old_list.length);
        }

        /* The value at index should match what we set */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));

        /* current_size must be >= length * item_size */
        size_t required_size = list.length * list.item_size;
        assert(list.current_size >= required_size);

        /* In static mode, current_size doesn't change */
        if (list.alloc == NULL) {
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
    } else {
        /* On failure:
         * - List should remain valid (already checked above)
         * - In static mode with out-of-bounds index, list is unchanged
         * - In dynamic mode, allocation failure may leave list partially changed
         *   but still valid
         */
        /* Length should not have increased beyond what's valid */
        assert(list.length <= old_list.length || list.current_size >= list.length * list.item_size);

        /* If static (no allocator), nothing should have changed */
        if (list.alloc == NULL) {
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
            assert(list.length == old_list.length);
        }
    }
}
