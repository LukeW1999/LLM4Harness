#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Non-deterministic value to set (must be readable for item_size bytes) */
    /* Allocate a buffer for val */
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

    /* Fields that never change regardless of outcome */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The data pointer may have changed (if realloc happened in dynamic mode)
         * - current_size may have grown
         * - length is updated: if index >= old length, new length = index + 1
         *   otherwise length stays the same
         * - The value at index is now val (memcpy'd)
         */

        /* current_size must be >= required to hold index */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* Length update: if index was >= old length, length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }

        /* data must be non-null since we successfully wrote */
        assert(list.data != NULL);

        /* The value at index matches val */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));

    } else {
        /* On failure:
         * - The list should remain valid (already asserted above)
         * - length should not have changed (ensure_capacity failed before any write)
         * - current_size should not have changed (or may have stayed same)
         * - In static mode: current_size unchanged, length unchanged
         * - In dynamic mode: allocation may have failed, list state preserved
         */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    }
}
