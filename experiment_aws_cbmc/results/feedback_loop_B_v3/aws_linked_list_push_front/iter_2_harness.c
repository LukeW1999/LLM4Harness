#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* 2. Bound the structure (limits CBMC state space) */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Allocate pointer members */
    /* No additional allocation needed for node as it's a simple struct */

    /* 4. Assume validity precondition (infer from how function uses the struct) */
    /* The list is already assumed to be valid */

    /* 5. Save old state (for checking immutability) */
    struct aws_linked_list old_list = list;

    /* 6. Assume function-specific preconditions (infer from code logic) */
    /* The node does not need to be part of any list before being pushed front */
    aws_linked_list_node_reset(&node);

    /* 7. Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* 8. Assert postconditions (infer from what the function guarantees) */
    /* Postcondition: The list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition: The node is now the first node in the list */
    assert(list.head.next == &node);

    /* Postcondition: The node's previous pointer is the head */
    assert(node.prev == &list.head);

    /* Postcondition: The node's next pointer is the old first node */
    assert(node.next == old_list.head.next);

    /* Postcondition: The old first node's previous pointer is the new node */
    if (old_list.head.next != &list.tail) {
        assert(old_list.head.next->prev == &node);
    }
}
