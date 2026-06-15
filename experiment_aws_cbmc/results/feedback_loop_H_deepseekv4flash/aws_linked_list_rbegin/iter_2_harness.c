#include <aws/common/linked_list.h>
#include <stdlib.h>
#include <proof_helpers/nondet.h>

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    
    struct aws_linked_list_node *node1 = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node1 != NULL);
    
    // Set up a valid non-empty circular doubly linked list with one node
    list.head.prev = &list.tail;
    list.head.next = node1;
    node1->next = &list.tail;
    node1->prev = &list.head;
    list.tail.next = &list.head;
    list.tail.prev = node1;
    
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);
    
    assert(rval == list.tail.prev);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);
    assert(aws_linked_list_is_valid(&list));
    assert(rval->next == &list.tail);
}
