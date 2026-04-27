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
         * - list.data must be non-null (memcpy was performed)
         * - list.length >= index + 1
         * - current_size >= list.length * list.item_size
         */
        assert(list.data != NULL);
        /* If index was >= old length, length becomes index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within existing bounds */
            assert(list.length == old_list.length);
        }
        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* On failure:
         * - ensure_capacity failed, list should still be valid
         * - length should not have increased beyond what it was
         *   (set_at returns early if ensure_capacity fails)
         * - The list remains valid (checked above)
         * - item_size and alloc unchanged (checked above)
         */
        /* length should be unchanged on failure path */
        assert(list.length == old_list.length);
        /* current_size should be unchanged on failure path */
        assert(list.current_size == old_list.current_size);
        /* data pointer should be unchanged on failure path */
        assert(list.data == old_list.data);
    }
}
