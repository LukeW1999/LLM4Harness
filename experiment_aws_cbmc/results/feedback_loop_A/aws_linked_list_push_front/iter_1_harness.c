#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_remove_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node node;
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;

    /* 3. Call function under test */
    aws_linked_list_remove(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* For aws_linked_list_remove, there is no return value, so we assume it always succeeds */
    assert(node.next == NULL && node.prev == NULL);
    assert(node.next != old_node.next || node.prev != old_node.prev); // Node is removed

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.next != &node);
    assert(list.tail.prev != &node);

    /* 6. Assert validity invariants always holds */
    assert(aws_linked_list_is_valid(&list));
}
