#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;

    /* Bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure data is allocated if current_size > 0 */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume list is valid before call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state for comparison */
    struct aws_array_list old = list;

    /* Call the function */
    aws_array_list_clear(&list);

    /* Postconditions */
    assert(list.length == 0);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
