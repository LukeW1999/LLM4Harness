#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;

    /* Bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure data is allocated if current_size > 0 */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume list is valid before operation */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Call function */
    int result = aws_array_list_pop_back(&list);

    /* Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success implies the list had at least one element */
        assert(old.length > 0);
        /* Length decreased by exactly one */
        assert(list.length == old.length - 1);
        /* Other fields unchanged */
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    } else {
        /* Failure implies the list was empty */
        assert(old.length == 0);
        /* List unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    }

    /* Postcondition: list invariant holds */
    assert(aws_array_list_is_valid(&list));
}
