#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;
    struct aws_linked_list_node prev_node;

    node.next = &next_node;
    node.prev = &prev_node;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node old_node = node;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path */
    assert(result == &next_node);

    /* Failure path not applicable as the function always succeeds */

    /* 5. Assert fields that must NOT change regardless of result */
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);

    /* 6. Assert validity invariant always holds */
    /* No specific validity invariant for aws_linked_list_node_next */
}
