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

    /* 2. Create a non-deterministic index */
    size_t index;

    /* 3. Create a non-deterministic val pointer with enough readable memory */
    /* val must point to readable memory of list->item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state before calling */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must NOT change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The memory at index*item_size in list->data contains val
         * - If index >= old length, length becomes index + 1
         * - current_size may have grown (if dynamic reallocation happened)
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* Length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same or grows */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient for the new length */
        assert(list.current_size >= list.length * list.item_size);

    } else {
        /* On failure:
         * - The list should remain valid (already asserted above)
         * - In static mode, AWS_ERROR_INVALID_INDEX is raised
         * - The list state should be unchanged on failure
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    }
}
