#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_sort_harness() {
    struct aws_array_list list;

    /* Nondeterministically bound the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Nondeterministic comparator function pointer (must be non-NULL) */
    aws_array_list_comparator_fn *compare_fn;
    __CPROVER_assume(compare_fn != NULL);

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);

    /* Call the function under test */
    aws_array_list_sort(&list, compare_fn);

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Unchanged fields (fields not modified by sort) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    /* The content is rearranged, but we cannot assert specific order because comparator is nondeterministic.
       However, we can claim that the memory region still holds the same total bytes (not checked here). */
}
