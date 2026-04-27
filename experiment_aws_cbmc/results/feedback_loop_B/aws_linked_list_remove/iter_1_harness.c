#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    struct aws_linked_list_node prev_node;
    struct aws_linked_list_node next_node;

    node.prev = &prev_node;
    node.next = &next_node;
    prev_node.next = &node;
    next_node.prev = &node;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;
    struct aws_linked_list_node old_prev_node = prev_node;
    struct aws_linked_list_node old_next_node = next_node;

    /* 3. Call function under test */
    aws_linked_list_remove(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* In this case, the function does not return a value, so we assume it always succeeds if preconditions are met */
    assert(prev_node.next == &next_node);
    assert(next_node.prev == &prev_node);
    assert(node.next == NULL);
    assert(node.prev == NULL);

    /* 5. Assert fields that must NOT change regardless of result */
    /* The function only modifies the prev and next pointers of the node and its neighbors */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6. Assert validity invariants always holds */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_is_valid(&prev_node));
    assert(aws_linked_list_node_is_valid(&next_node));
}
