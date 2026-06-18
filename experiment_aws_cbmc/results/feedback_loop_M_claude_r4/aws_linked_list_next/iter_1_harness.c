#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_next_harness() {
    /* 1. Set up a linked list node with a non-deterministic next pointer */
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;

    /* node.next points to next_node (a valid node) */
    node.next = &next_node;
    node.prev = NULL; /* prev not relevant for this function */

    /* next_node can have arbitrary pointers */
    next_node.next = NULL;
    next_node.prev = &node;

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node.next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* 4. Assert postconditions */
    /* The function returns node->next */
    assert(result == old_next);
    assert(result == &next_node);

    /* 5. Assert node is unchanged */
    assert(node.next == old_next);
}
