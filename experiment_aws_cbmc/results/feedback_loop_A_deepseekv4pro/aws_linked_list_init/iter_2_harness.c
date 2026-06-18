#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 2

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    node.prev = NULL;
    node.next = NULL;

    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;

    aws_linked_list_push_back(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_tail_prev);

    if (old_head_next == &list.tail) {
        assert(list.head.next == &node);
    } else {
        assert(list.head.next == old_head_next);
    }

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
