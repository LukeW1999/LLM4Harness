#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness() {
    /* Build a minimal linked list with one middle node */
    struct aws_linked_list list;
    struct aws_linked_list_node node; /* the node to be removed */

    /* Set up head and tail sentinel nodes (they are part of the list) */
    list.head.next = &node;
    list.head.prev = NULL;          /* head.prev is typically unused */
    list.tail.next = NULL;          /* tail.next is typically unused */
    list.tail.prev = &node;

    /* Wire the middle node */
    node.prev = &list.head;
    node.next = &list.tail;

    /* Ensure the list is valid (deep check) */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Save old state of the node (prev/next pointers) */
    struct aws_linked_list_node old_node = node;

    /* Call the function under test */
    aws_linked_list_remove(&node);

    /* Postconditions:
     * 1. The removed node's prev and next are NULL (reset).
     * 2. The surrounding nodes (head and tail) now point to each other.
     * 3. The list remains valid.
     */

    /* Node reset check */
    assert(node.prev == NULL);
    assert(node.next == NULL);

    /* List connectivity after removal: head and tail are directly linked */
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);

    /* The list must still satisfy all invariants */
    assert(aws_linked_list_is_valid_deep(&list));

    /* Optional: check that the old_node's pointers pointed to head and tail */
    /* (These are not modified by the function, just for completeness) */
    assert(old_node.prev == &list.head);
    assert(old_node.next == &list.tail);
}
