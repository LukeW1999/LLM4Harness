#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* nondet list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* nondet node to add */
    struct aws_linked_list_node node;
    /* ensure node is not already in the list */
    __CPROVER_assume(node.prev == NULL && node.next == NULL);

    /* save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* call the function */
    aws_linked_list_push_back(&list, &node);

    /* postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_tail_prev);

    if (old_tail_prev != &list.head) {
        assert(old_tail_prev->next == &node);
    } else {
        assert(list.head.next == &node);
    }

    /* additional invariants */
    assert(list.tail.next == NULL);
    assert(list.head.prev == NULL);
}
