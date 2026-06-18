#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    /* Allocate a linked list */
    struct aws_linked_list list;

    /* Allocate two nodes */
    struct aws_linked_list_node *node = malloc(sizeof *node);
    __CPROVER_assume(node != NULL);
    struct aws_linked_list_node *prev_node = malloc(sizeof *prev_node);
    __CPROVER_assume(prev_node != NULL);

    /* Set up list sentinel nodes */
    list.head.prev = NULL;
    list.head.next = prev_node;
    list.tail.prev = node;
    list.tail.next = NULL;

    /* Link the two nodes */
    prev_node->prev = &list.head;
    prev_node->next = node;
    node->prev = prev_node;
    node->next = &list.tail;

    /* Assume the list is valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list_node old_prev_node = *prev_node;
    struct aws_linked_list old_list = list;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postconditions */
    assert(result == old_node.prev);
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
    assert(prev_node->next == old_prev_node.next);
    assert(prev_node->prev == old_prev_node.prev);

    /* List must remain valid and unchanged */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
}
