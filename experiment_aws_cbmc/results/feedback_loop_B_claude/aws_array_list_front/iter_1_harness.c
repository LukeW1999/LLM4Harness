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

    /* 3. Allocate a val buffer of item_size bytes */
    const void *val = malloc(list.item_size);
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
         * - capacity (current_size) may have grown (dynamic) or stayed same (static)
         * - length is updated: if index >= old length, new length = index + 1
         * - data pointer may have changed (realloc in dynamic mode)
         */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
        /* current_size must be >= length * item_size */
        size_t required_size;
        int no_overflow = !aws_mul_size_checked(list.length, list.item_size, &required_size);
        assert(no_overflow);
        assert(list.current_size >= required_size);
        /* data must be non-null since we wrote something */
        assert(list.data != NULL);
    } else {
        /* On failure:
         * - list should still be valid (postcondition in implementation)
         * - item_size and alloc unchanged (already asserted above)
         * - length should not have changed beyond what ensure_capacity may have done
         *   but since ensure_capacity failed, length stays the same
         */
        assert(list.length == old_list.length);
        /* current_size: if ensure_capacity failed, it may or may not have changed
         * but the list must still be valid */
        assert(aws_array_list_is_valid(&list));
    }
}
