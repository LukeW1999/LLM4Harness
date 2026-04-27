#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* 2. Bound the structure (limits CBMC state space) */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 3. Allocate pointer members */
    /* No pointer members to allocate for aws_linked_list_node */

    /* 4. Assume validity precondition (infer from how function uses the struct) */
    /* The list is already allocated, so we assume it's valid */
    /* The node does not need any specific allocation or assumption */

    /* 5. Save old state (for checking immutability) */
    struct aws_linked_list old_list = list;

    /* 6. Assume function-specific preconditions (infer from code logic) */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&list, sizeof(struct aws_linked_list)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&node, sizeof(struct aws_linked_list_node)));
    __CPROVER_assume(list.head.next != NULL);
    __CPROVER_assume(list.tail.prev != NULL);
    __CPROVER_assume(node.next == NULL);
    __CPROVER_assume(node.prev == NULL);

    /* 7. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 8. Assert postconditions (infer from what the function guarantees) */
    /* Success case: node is inserted before the tail */
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev->next == &node);
    assert(node.prev == old_list.tail.prev);
    assert(old_list.tail.prev->next == &node);

    /* Invariant: list is still valid */
    assert(AWS_MEM_IS_WRITABLE(&list, sizeof(struct aws_linked_list)));
    assert(AWS_MEM_IS_WRITABLE(&node, sizeof(struct aws_linked_list_node)));
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);
}
