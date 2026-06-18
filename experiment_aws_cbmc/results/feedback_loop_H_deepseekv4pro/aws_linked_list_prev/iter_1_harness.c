#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    /* A single node with arbitrary content */
    struct aws_linked_list_node node;
    struct aws_linked_list_node *ptr = &node;

    /* The node must be non-null (guaranteed by address-of) and readable.
     * Leave its next/prev pointers unconstrained; CBMC will treat them as any value. */

    /* Save the original state for immutability checks */
    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_prev = node.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(ptr);

    /* Postcondition 1: return value equals node->prev */
    assert(result == node.prev);

    /* Postcondition 2: the node itself is not modified (const function) */
    assert(node.next == old_next);
    assert(node.prev == old_prev);
}
