#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    __CPROVER_assume(AWS_MEM_IS_READABLE(&node, sizeof(struct aws_linked_list_node)));
    __CPROVER_assume(node.prev != NULL);
    __CPROVER_assume(aws_linked_list_node_is_valid(node.prev));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* In this case, the function does not have a return code, so we only handle the success path */
    if (result != NULL) {
        assert(result == node.prev);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
    if (result != NULL) {
        assert(aws_linked_list_node_is_valid(result));
    }
}
