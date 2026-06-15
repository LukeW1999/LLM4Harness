#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    /* Create two nodes and link them in a circular list */
    struct aws_linked_list_node node1, node2;

    node1.prev = &node2;
    node1.next = &node2;
    node2.prev = &node1;
    node2.next = &node1;

    /* Ensure the pointers are non‑NULL (trivially true, but kept for clarity) */
    __CPROVER_assume(&node1 != NULL);
    __CPROVER_assume(&node2 != NULL);

    /* Save the original state of node1 */
    struct aws_linked_list_node old = node1;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node1);

    /* Post‑conditions */
    assert(result == old.prev);
    assert(node1.prev == old.prev);
    assert(node1.next == old.next);
}
