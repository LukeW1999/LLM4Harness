#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness(void) {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;

    /* Set up a valid bidirectional link between node and next_node */
    node.next = &next_node;
    node.prev = NULL; /* prev not relevant for this test */
    next_node.prev = &node;
    next_node.next = NULL;

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_prev = node.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* 4. Assert postconditions */
    /* The function returns node->next */
    assert(result == old_next);
    assert(result == &next_node);

    /* 5. Assert unchanged fields - node should not be modified */
    assert(node.next == old_next);
    assert(node.prev == old_prev);

    /* 6. Assert the returned pointer is valid (non-null since we set it up) */
    assert(result != NULL);

    /* 7. Assert the linkage is still valid */
    assert(node.next == &next_node);
    assert(next_node.prev == &node);
}
