#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node node;
    struct aws_linked_list_node *next_node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    node.next = next_node;
    node.prev = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node.prev != NULL);

    /* Assume the node is part of a valid linked list */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node old_node = node;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == next_node) {
        /* Success path */
        assert(result == node.next);
    } else {
        /* Failure path: should not occur as the function is deterministic and always returns node->next */
        assert(false);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_node_next_is_valid(&node));

    free(next_node);
    free(node.prev);
}
