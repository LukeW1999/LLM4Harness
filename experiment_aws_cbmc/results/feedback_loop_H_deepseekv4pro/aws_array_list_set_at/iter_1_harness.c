#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    size_t index;
    uint8_t *val;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the data member is allocated */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate a buffer for the value to be copied, sized to match the list's element size */
    val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save the old state for postcondition checks */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* Call the function under verification */
    int result = aws_array_list_set_at(&list, val, index);

    /* Invariants that must hold regardless of success or failure */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /*
         * On success:
         * - The element at 'index' must equal 'val'.
         * - If 'index' was beyond the old length, the length is updated to index+1.
         * - Otherwise the length remains unchanged.
         */
        assert(index < list.current_size);
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }
    } else {
        /*
         * On failure:
         * - The length must not have changed.
         * - The list remains valid (already asserted).
         * - The data pointer may have changed if ensure_capacity succeeded but the
         *   subsequent length update overflowed; in that case the element at 'index'
         *   was already written, so we cannot assert full immutability.
         */
        assert(list.length == old_length);
    }
}
