#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* 1. Allocate and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node to be pushed back. It must not be NULL. */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    /* The node is not yet in any list; its prev/next pointers are arbitrary.
       We do not assume anything about them, except that they do not cause
       the list validity check to fail (the node is not linked in). */

    /* 3. Save old state: the tail's previous node before insertion */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert the list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* 6. Assert that the new node is now the last element */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);

    /* 7. Assert that the previous tail->prev now links forward to node */
    if (old_tail_prev != NULL) {
        assert(old_tail_prev->next == node);
    }

    /* 8. Assert that the node's previous pointer points to the old tail->prev */
    assert(node->prev == old_tail_prev);

    /* 9. Assert that the head is unchanged. For an empty list, head.next becomes node,
          but we check through the list validity: the head.next pointer is correct
          as part of the valid list. However, we can also assert that if the list
          was empty, head.next becomes node. That is already covered by validity.
          Optionally: */
    if (old_tail_prev == &list.head) {
        /* List was empty before push_back */
        assert(list.head.next == node);
    }

    /* 10. Assert that the node is now in a list (bidirectional checks) */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));

    /* 11. Validity invariants on the list itself */
    assert(aws_linked_list_is_valid(&list));
}
