#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node1;
    struct aws_linked_list_node node2;

    // Initialize list as empty (head and tail sentinels)
    list.head.next = &list.tail;
    list.head.prev = &list.head;
    list.tail.next = &list.tail;
    list.tail.prev = &list.head;

    // Push back node1 into empty list
    aws_linked_list_push_back(&list, &node1);

    // Postconditions after first push
    assert(list.head.next == &node1);
    assert(node1.prev == &list.head);
    assert(node1.next == &list.tail);
    assert(list.tail.prev == &node1);

    // Push back node2 into non-empty list
    aws_linked_list_push_back(&list, &node2);

    // Postconditions after second push
    assert(list.head.next == &node1);
    assert(node1.prev == &list.head);
    assert(node1.next == &node2);
    assert(node2.prev == &node1);
    assert(node2.next == &list.tail);
    assert(list.tail.prev == &node2);
}
