#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* Non-deterministic linked list and node */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Ensure the list is properly allocated and valid */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The node must be non-NULL; it is a stack variable so already writable */
    __CPROVER_assume(&node != NULL);

    /* Save the old tail.prev before the call */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function */
    aws_linked_list_push_back(&list, &node);

    /* Postcondition: list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition: node is the new last element */
    assert(list.tail.prev == &node);

    /* The node's next pointer must point to the tail sentinel */
    assert(node.next == &list.tail);

    /* The node's prev pointer must point to the previous last element */
    assert(node.prev == old_tail_prev);

    /* If old_tail_prev was not head (i.e., list non-empty), its next must now point to node */
    /* Note: old_tail_prev cannot be NULL because tail.prev initially points to head (sentinel) or a valid node */
    assert(old_tail_prev->next == &node);
}
