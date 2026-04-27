#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness(void) {
    /* Declare and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create a node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_front = list.head.next;

    /* Call function under test */
    aws_linked_list_push_front(&list, node);

    /* Postcondition 1: List remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 2: List is non-empty */
    assert(!aws_linked_list_empty(&list));

    /* Postcondition 3: Node is now at the front */
    assert(list.head.next == node);

    /* Postcondition 4: Node's prev points to head */
    assert(node->prev == &list.head);

    /* Postcondition 5: Node's next points to old front */
    assert(node->next == old_front);

    /* Postcondition 6: Old front's prev points to node */
    assert(old_front->prev == node);

    /* Postcondition 7: head.prev is still NULL */
    assert(list.head.prev == NULL);

    /* Postcondition 8: tail.next is still NULL */
    assert(list.tail.next == NULL);
}
