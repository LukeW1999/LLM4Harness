#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    __CPROVER_assume(&node != NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_first = list.head.next;

    /* 3. Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since the function does not have a return value, we assume it always succeeds */
    assert(list.head.next == &node);   // push_front: head now points to new node
    assert(node.next == old_first);     // new node links forward to previous first
    assert(node.prev == &list.head);    // new node points to head

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.tail.prev == old_list.tail.prev || list.tail.prev == &node);
    assert(list.tail.next == old_list.tail.next);
    assert(list.head.prev == old_list.head.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_is_in_list(&node));
}
