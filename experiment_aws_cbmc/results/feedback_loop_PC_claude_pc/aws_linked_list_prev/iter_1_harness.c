#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Set up a linked list node with non-deterministic prev pointer */
    struct aws_linked_list_node node;
    struct aws_linked_list_node prev_node;

    /* node.prev points to prev_node (a valid node) */
    node.prev = &prev_node;
    node.next = NULL; /* next is not used by this function */

    /* prev_node can have arbitrary pointers */
    prev_node.next = &node; /* bidirectional link */
    prev_node.prev = NULL;  /* not used by this function */

    /* 2. Save old state before calling */
    struct aws_linked_list_node *old_prev = node.prev;
    struct aws_linked_list_node *old_next = node.next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* 4. Assert postconditions */

    /* RETURN: result must equal node->prev */
    assert(result == old_prev);
    assert(result == &prev_node);

    /* FRAME: node's fields must not have changed */
    assert(node.prev == old_prev);
    assert(node.next == old_next);

    /* FRAME: prev_node's fields must not have changed */
    assert(prev_node.next == &node);
    assert(prev_node.prev == NULL);

    /* INVARIANTS: result is non-null (we set it to &prev_node) */
    assert(result != NULL);

    /* The returned pointer is exactly node->prev */
    assert(result == node.prev);
}
