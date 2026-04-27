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

    /* 3. Non-deterministic val — must be readable for item_size bytes */
    void *val;
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
         * - The data was written at index * item_size
         * - length is updated if index >= old length: length = index + 1
         * - current_size may have grown (if dynamic reallocation happened)
         */

        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* current_size must be sufficient to hold length items */
        assert(list.current_size >= list.length * list.item_size);

        /* If index was within old bounds, length stays the same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length was updated to index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size can only grow or stay the same */
        assert(list.current_size >= old_list.current_size);

        /* data pointer must be non-null since we have data */
        assert(list.data != NULL);

    } else {
        /* On failure:
         * - ensure_capacity failed (dynamic realloc failed or static out of bounds)
         * - list state should remain valid (as per postcondition in implementation)
         * - length should not have changed (memcpy never happened)
         * - For static lists: current_size and data unchanged
         * - For dynamic lists: alloc may have failed, but list remains valid
         */

        /* length should not have increased beyond what it was */
        /* On failure, the set_at returns early before modifying length */
        assert(list.length == old_list.length);

        /* For static lists (no allocator), current_size and data are unchanged */
        if (list.alloc == NULL) {
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
    }
}
