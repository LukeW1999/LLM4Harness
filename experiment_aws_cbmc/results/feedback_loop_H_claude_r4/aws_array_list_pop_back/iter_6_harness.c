#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* 3. Provide a val pointer to receive the popped element */
    void *val = NULL;
    if (list.item_size > 0) {
        val = bounded_malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list is valid after pop_back");

    /* item_size never changes */
    __CPROVER_assert(list.item_size == old_item_size, "item_size unchanged");

    /* alloc never changes */
    __CPROVER_assert(list.alloc == old_alloc, "alloc unchanged");

    /* current_size never changes (pop_back does not shrink allocation) */
    __CPROVER_assert(list.current_size == old_current_size, "current_size unchanged");

    /* data pointer never changes */
    __CPROVER_assert(list.data == old_data, "data pointer unchanged");

    if (result == AWS_OP_SUCCESS) {
        /* On success, list must have had at least one element */
        __CPROVER_assert(old_length > 0, "success implies non-empty list");

        /* On success, length decreases by 1 */
        __CPROVER_assert(list.length == old_length - 1, "length decremented on success");
    } else {
        /* On failure, list was empty */
        __CPROVER_assert(old_length == 0, "failure implies empty list");

        /* Length remains 0 */
        __CPROVER_assert(list.length == 0, "length stays 0 on failure");
    }
}
