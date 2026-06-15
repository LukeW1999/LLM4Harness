#include <aws/common/linked_list.h>

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node1, node2;
    
    // Initialize list with two nodes
    list.head.prev = NULL;
    list.head.next = &node1;
    node1.prev = &list.head;
    node1.next = &node2;
    node2.prev = &node1;
    node2.next = &list.tail;
    list.tail.prev = &node2;
    list.tail.next = NULL;

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *end = aws_linked_list_end(&list);

    assert(end == &list.tail);
    assert(end != NULL);

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    assert(aws_linked_list_is_valid(&list));
}
