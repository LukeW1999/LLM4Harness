#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness() {
    /* Create a non-deterministic linked list with bounded size */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Choose a node to remove from the list (e.g., the last node) */
    struct aws_linked_list_node *node = aws_linked_list_back(&list);
    /* The node must be a valid element (not head or tail) */
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));
    __CPROVER_assume(node != &list.head && node != &list.tail);

    /* Save pointers that will be modified */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Perform the removal */
    aws_linked_list_remove(node);

    /* Postconditions */
    /* 1. The node's pointers are reset to NULL */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 2. The surrounding nodes now point to each other */
    if (old_prev != NULL) {
        assert(old_prev->next == old_next);
    }
    if (old_next != NULL) {
        assert(old_next->prev == old_prev);
    }

    /* 3. The list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* 4. The list's head and tail node structures themselves are unchanged */
    /* (Implicitly checked by aws_linked_list_is_valid, but we can also assert
     * that the head and tail pointers have not been overwritten incorrectly) */
    assert(list.head.prev == NULL); /* head.prev should always be NULL */
    assert(list.tail.next == NULL); /* tail.next should always be NULL */
}
