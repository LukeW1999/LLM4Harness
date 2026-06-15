#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_front_harness() {
    /* nondeterministic allocation and initialization */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* precondition: list is valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* save old state (only top-level pointers, since list is not modified) */
    struct aws_linked_list old_list = list;

    /* call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* postcondition: result points to the first node */
    assert(result == list.head.next);

    /* postcondition: list structure is unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* postcondition: result is a valid node (not NULL, not the tail sentinel) */
    assert(result != NULL);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));

    /* invariant: list remains valid */
    assert(aws_linked_list_is_valid_deep(&list));
}
