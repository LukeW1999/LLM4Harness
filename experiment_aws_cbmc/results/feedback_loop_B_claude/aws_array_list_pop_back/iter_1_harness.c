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

    /* Fields that never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The data at index*item_size should match val
         * - length should be at least index+1
         * - current_size may have grown (dynamic) or stayed same (static)
         */
        assert(list.length > index);
        /* length is either old length (if index < old length) or index+1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
        /* current_size can only grow or stay the same */
        assert(list.current_size >= old_list.current_size);
        /* data pointer is non-null since we wrote something */
        assert(list.data != NULL);
        /* The value was actually copied */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));
    } else {
        /* On failure:
         * - list should still be valid (postcondition from implementation)
         * - length should not have changed (ensure_capacity failed before write)
         * - current_size should not have changed
         */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    }
}
