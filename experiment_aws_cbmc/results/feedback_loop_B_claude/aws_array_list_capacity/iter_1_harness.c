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
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - current_size may have grown (if dynamic realloc happened)
         * - length is updated: if index >= old length, new length = index + 1
         *   otherwise length stays the same
         * - data pointer may have changed (realloc)
         * - The value at index was written
         */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);
        /* data must be non-null since we wrote something */
        assert(list.data != NULL);
    } else {
        /* On failure:
         * - The list should still be valid (postcondition from implementation)
         * - item_size and alloc unchanged (already asserted above)
         * - length should not have increased beyond what it was
         *   (ensure_capacity failed, so no write happened)
         * - current_size may or may not have changed depending on where failure occurred
         *   but the list remains valid
         */
        /* length should be unchanged on failure (no successful write) */
        assert(list.length == old_list.length);
        /* current_size unchanged on failure (realloc failed or static mode) */
        assert(list.current_size == old_list.current_size);
        /* data pointer unchanged on failure */
        assert(list.data == old_list.data);
    }
}
