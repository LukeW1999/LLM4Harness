#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and set up the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 3. Optionally capture the back element if list is non-empty */
    void *val = NULL;
    if (list.item_size > 0 && list.length > 0) {
        val = bounded_malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list is valid after pop_back");

    /* item_size never changes */
    __CPROVER_assert(list.item_size == old_item_size, "item_size unchanged");

    /* alloc never changes */
    __CPROVER_assert(list.alloc == old_alloc, "alloc unchanged");

    /* current_size never changes (pop_back does not reallocate) */
    __CPROVER_assert(list.current_size == old_current_size, "current_size unchanged");

    if (result == AWS_OP_SUCCESS) {
        /* On success: list was non-empty, length decreased by 1 */
        __CPROVER_assert(old_length > 0, "old_length > 0 on success");
        __CPROVER_assert(list.length == old_length - 1, "length decremented on success");
    } else {
        /* On failure: list was empty, length unchanged */
        __CPROVER_assert(old_length == 0, "old_length == 0 on failure");
        __CPROVER_assert(list.length == old_length, "length unchanged on failure");
    }
}
