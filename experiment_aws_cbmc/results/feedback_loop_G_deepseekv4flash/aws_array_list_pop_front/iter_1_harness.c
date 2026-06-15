#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_harness() {
    /* nondeterministic list with bounded allocation */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;

    int result = aws_array_list_pop_front(&list);

    /* postconditions */
    if (old.length == 0) {
        /* failure: list was empty */
        assert(result != AWS_OP_SUCCESS);
        /* all fields unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
    } else {
        /* success: list had at least one element */
        assert(result == AWS_OP_SUCCESS);
        /* length decreases by 1 */
        assert(list.length == old.length - 1);
        /* size does not change (pop does not shrink allocation) */
        assert(list.current_size == old.current_size);
        /* other fields unchanged */
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
    }

    /* validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
