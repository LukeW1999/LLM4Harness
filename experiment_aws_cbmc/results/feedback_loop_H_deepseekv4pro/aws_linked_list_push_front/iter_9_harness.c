#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Reset node so it is not part of any list */
    aws_linked_list_node_reset(&node);

    /* Initialize list to empty valid state */
    aws_linked_list_init(&list);

    /* The old head is the first real node, i.e., head.next (could be the tail sentinel) */
    struct aws_linked_list_node *old_head = list.head.next;

    /* Call the function under test */
    aws_linked_list_push_front(&list, &node);

    /* Postcondition: head.next points to the new node */
    assert(list.head.next == &node);
    /* node's prev is the head sentinel */
    assert(node.prev == &list.head);

    /* node->next points to the old head */
    assert(node.next == old_head);

    /* If the list was non-empty before the push, the old head's prev must now be the new node */
    if (old_head != &list.tail) {
        assert(old_head->prev == &node);
    } else {
        /* The list was empty, so tail.prev must now point to the new node */
        assert(list.tail.prev == &node);
    }
}
