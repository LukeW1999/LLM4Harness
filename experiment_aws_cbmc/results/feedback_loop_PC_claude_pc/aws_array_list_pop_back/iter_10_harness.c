#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;

    /* 3. Allocate val buffer to receive the popped item */
    void *val = bounded_malloc(MAX_ITEM_SIZE > 0 ? MAX_ITEM_SIZE : 1);

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* item_size must never change */
    __CPROVER_assert(list.item_size == old_item_size, "item_size unchanged");

    /* alloc must never change */
    __CPROVER_assert(list.alloc == old_alloc, "alloc unchanged");

    /* current_size must never change */
    __CPROVER_assert(list.current_size == old_current_size, "current_size unchanged");

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must be valid */
        __CPROVER_assert(aws_array_list_is_valid(&list), "list valid on success");

        /* old_length must have been > 0 for success */
        __CPROVER_assert(old_length > 0, "old_length > 0 on success");

        /* On success, length must have decreased by 1 */
        __CPROVER_assert(list.length == old_length - 1, "length decremented on success");

    } else {
        /* On failure, the list must still be valid */
        __CPROVER_assert(aws_array_list_is_valid(&list), "list valid on failure");

        /* On failure, length should not have changed */
        __CPROVER_assert(list.length == old_length, "length unchanged on failure");

        /* Failure implies the list was empty */
        __CPROVER_assert(old_length == 0, "old_length == 0 on failure");
    }

    /* Validity invariant always holds */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list always valid");
}
