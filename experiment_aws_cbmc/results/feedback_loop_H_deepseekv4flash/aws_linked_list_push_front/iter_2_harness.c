#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    /* Create a list with two nodes (head and tail are sentinels) */
    struct aws_linked_list list;
    struct aws_linked_list_node node1;
    struct aws_linked_list_node node2;
    struct aws_linked_list_node new_node;

    /* Initialize list as empty */
    aws_linked_list_init(&list);

    /* Insert node1 after head */
    node1.next = list.head.next;          /* = &list.tail */
    node1.prev = &list.head;
    list.head.next = &node1;
    list.tail.prev = &node1;

    /* Insert node2 after node1 */
    node2.next = node1.next;              /* = &list.tail */
    node2.prev = &node1;
    node1.next = &node2;
    list.tail.prev = &node2;

    /* List is now: head <-> node1 <-> node2 <-> tail */

    /* For the new node, we assume it is not in any list */
    new_node.next = NULL;
    new_node.prev = NULL;

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state for postcondition checks */
    struct aws_linked_list_node old_head = list.head;
    struct aws_linked_list_node old_node1 = node1;
    struct aws_linked_list_node old_node2 = node2;

    /* Call the function under test */
    aws_linked_list_push_front(&list, &new_node);

    /* Postcondition 1: List remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 2: The new node is at the front */
    assert(list.head.next == &new_node);
    assert(new_node.prev == &list.head);
    assert(new_node.next == &node1);

    /* Postcondition 3: The old front node's prev points to new_node */
    assert(node1.prev == &new_node);

    /* Postcondition 4: The tail is unchanged */
    assert(list.tail.prev == &node2);

    /* Postcondition 5: The head prev and tail next remain NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* Postcondition 6: Node1 and node2 remain valid */
    assert(aws_linked_list_node_prev_is_valid(&node1));
    assert(aws_linked_list_node_next_is_valid(&node1));
    assert(aws_linked_list_node_prev_is_valid(&node2));
    assert(aws_linked_list_node_next_is_valid(&node2));

    /* Postcondition 7: The new node is valid */
    assert(aws_linked_list_node_prev_is_valid(&new_node));
    assert(aws_linked_list_node_next_is_valid(&new_node));

    /* Postcondition 8: The list head and tail pointers are unchanged */
    assert(&list.head == &list.head);
    assert(&list.tail == &list.tail);
}
