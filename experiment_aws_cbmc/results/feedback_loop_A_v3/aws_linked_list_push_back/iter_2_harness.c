#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_linked_list_is_bounded(&list, MAX_LINKED_LIST_ITEM_ALLOCATION));

    /* 3. Allocate pointer members */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Save old state (for checking immutability) */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_last_node = list.tail.prev;

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    __CPROVER_assume(&node != NULL);

    /* 7. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 8. Assert postconditions (from Ensures: annotations) */
    assert(aws_linked_list_is_valid(&list));  // invariant always holds
    assert(list.tail.prev == &node);          // node is the new last element
    assert(node.prev == old_last_node);       // old last is now second-to-last
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
}
