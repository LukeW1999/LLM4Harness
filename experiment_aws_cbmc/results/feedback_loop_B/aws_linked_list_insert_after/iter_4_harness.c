#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    struct aws_linked_list_node to_add;

    /* 2. Assume preconditions */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&node, sizeof(node)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&to_add, sizeof(to_add)));
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(node.next->prev == &node);
    __CPROVER_assume(aws_linked_list_node_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_is_valid(node.next));

    /* 3. Call function under test */
    aws_linked_list_insert_after(&node, &to_add);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path: node and to_add are modified */
    assert(node.next == &to_add);
    assert(to_add.prev == &node);
    assert(to_add.next == node.next);
    assert(node.next->prev == &to_add);

    /* Failure path: nothing written (not applicable here as the function does not fail) */

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to assert as the function only modifies node and to_add */

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_is_valid(&node));
    assert(aws_linked_list_node_is_valid(&to_add));
}
