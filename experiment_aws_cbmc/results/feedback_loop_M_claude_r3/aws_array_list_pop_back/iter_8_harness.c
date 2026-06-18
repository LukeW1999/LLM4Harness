#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/*
 * Harness for aws_array_list_pop_back
 */

void aws_array_list_pop_back_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up the output buffer to receive the popped element */
    size_t item_size = list.item_size;

    void *val = bounded_malloc(item_size);

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 4. Call the function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list is valid after pop_back");

    /* item_size never changes */
    __CPROVER_assert(list.item_size == old_item_size, "item_size unchanged");

    /* alloc never changes */
    __CPROVER_assert(list.alloc == old_alloc, "alloc unchanged");

    if (result == AWS_OP_SUCCESS) {
        /* On success: length decreased by 1 */
        __CPROVER_assert(old_length > 0, "old_length > 0 on success");
        __CPROVER_assert(list.length == old_length - 1, "length decreased by 1");

        /* current_size unchanged */
        __CPROVER_assert(list.current_size == old_current_size, "current_size unchanged");

        /* data pointer unchanged */
        __CPROVER_assert(list.data == old_data, "data pointer unchanged");

    } else {
        /* On failure: list must be empty */
        __CPROVER_assert(old_length == 0, "old_length == 0 on failure");

        /* list is unchanged */
        __CPROVER_assert(list.length == old_length, "length unchanged on failure");
        __CPROVER_assert(list.current_size == old_current_size, "current_size unchanged on failure");
        __CPROVER_assert(list.data == old_data, "data unchanged on failure");
    }
}
