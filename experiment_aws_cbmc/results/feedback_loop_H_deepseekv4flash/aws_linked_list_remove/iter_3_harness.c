#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness() {
    /* Construct a valid circular doubly linked list with one middle node */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Initialize the list sentinels for a valid circular linked list with one data node */
    list.head.next = &node;          /* head points to first data node */
    list.head.prev = &list.tail;    /* head.prev points to tail (circular closure) */
    list.tail.next = &list.head;    /* tail.next points to head (circular closure) */
    list.tail.prev = &node;         /* tail.prev points to last data node (the node) */

    /* Wire the data node */
    node.prev = &list.head;
    node.next = &list.tail;

    /* The list is now valid. We can optionally assert this, but it's not required.
       Instead, we call the function directly. */
    /* Save old state for verification */
    struct aws_linked_list_node old_node = node;

    /* Call the function under test */
    aws_linked_list_remove(&node);

    /* Postcondition checks */
    assert(node.prev == NULL);
    assert(node.next == NULL);

    /* The head and tail should now be directly linked (empty list) */
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);

    /* The list must remain valid */
    assert(aws_linked_list_is_valid_deep(&list));

    /* Optional: verify that the removed node's original pointers were correct */
    assert(old_node.prev == &list.head);
    assert(old_node.next == &list.tail);
}
