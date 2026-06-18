#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_push_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);

    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    node.next = NULL;
    node.prev = NULL;
    __CPROVER_assume(node.next == NULL);
    __CPROVER_assume(node.prev == NULL);

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;

    aws_linked_list_push_front(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == (was_empty ? &list.tail : old_head_next));

    if (was_empty) {
        assert(list.tail.prev == &node);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }
}
