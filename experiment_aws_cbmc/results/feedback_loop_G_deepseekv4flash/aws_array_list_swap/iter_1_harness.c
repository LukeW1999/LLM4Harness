#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness() {
    /* Non-deterministic data structures and indices */
    struct aws_array_list list;
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Indices must be within bounds according to function precondition */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Save old state for immutability checks */
    struct aws_array_list old_list = list;

    /* Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* Postcondition: list is still valid */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: all fields except element data remain unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);
}
