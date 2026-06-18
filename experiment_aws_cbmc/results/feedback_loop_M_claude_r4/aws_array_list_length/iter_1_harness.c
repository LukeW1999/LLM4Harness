#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Bound and create the index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Create a valid val pointer with item_size bytes readable */
    /* item_size is already bounded by MAX_ITEM_SIZE via aws_array_list_is_bounded */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state BEFORE calling */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must NOT change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(list.current_size >= old_list.current_size || result != AWS_OP_SUCCESS);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The data at index*item_size should match val
         * - length should be at least index+1
         * - current_size should be >= length * item_size
         */
        assert(list.length >= index + 1);
        assert(list.current_size >= list.length * list.item_size);
        /* The list remains valid */
        assert(aws_array_list_is_valid(&list));
        /* If index was already within old length, length stays the same or grows */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            assert(list.length == index + 1);
        }
        /* current_size can only grow or stay the same on success */
        assert(list.current_size >= old_list.current_size);
        /* alloc and item_size unchanged */
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
    } else {
        /* On failure:
         * - The list should still be valid
         * - length and current_size should be unchanged (no partial modifications)
         * - alloc and item_size unchanged
         */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }
}
