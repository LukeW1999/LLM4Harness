#include <aws/common/linked_list.h>
#include <stdlib.h>
#include <proof_helpers/nondet.h>

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    // Initialize a non-empty circular doubly linked list with one node
    list.head.prev = &list.tail;
    list.head.next = node;
    node->next = &list.tail;
    node->prev = &list.head;
    list.tail.next = &list.head;
    list.tail.prev = node;

    // Precondition: list is valid
    assert(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    // Postcondition: rval is the last element (tail.prev)
    assert(rval == list.tail.prev);
    // The list should remain valid
    assert(aws_linked_list_is_valid(&list));
    // The list structure should be unchanged
    assert(list.head.next == node);
    assert(list.head.prev == &list.tail);
    assert(list.tail.next == &list.head);
    assert(list.tail.prev == node);
    // rval's next should be the tail sentinel
    assert(rval->next == &list.tail);
}
