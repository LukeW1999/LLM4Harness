#include <aws/common/linked_list.h>
#include <stddef.h>

void aws_linked_list_remove_harness() {
    /* Create a minimal linked list with one node */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Initialize head and tail sentinels */
    list.head.prev = NULL;
    list.head.next = &node;
    list.tail.prev = &node;
    list.tail.next = NULL;

    /* Initialize the single node */
    node.prev = &list.head;
    node.next = &list.tail;

    /* Precondition: node is in the list and is not head/tail */
    /* (Automatically satisfied by construction) */

    /* Save old neighbors */
    struct aws_linked_list_node *old_prev = node.prev;
    struct aws_linked_list_node *old_next = node.next;

    /* Perform removal */
    aws_linked_list_remove(&node);

    /* Postconditions */
    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
}
