#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate a linked list structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    /* Re‑initialize to an empty list to avoid stray pre‑existing nodes */
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate three distinct nodes */
    struct aws_linked_list_node *n1 = malloc(sizeof *n1);
    struct aws_linked_list_node *n2 = malloc(sizeof *n2);
    struct aws_linked_list_node *n3 = malloc(sizeof *n3);
    __CPROVER_assume(n1 && n2 && n3);

    aws_linked_list_node_reset(n1);
    aws_linked_list_node_reset(n2);
    aws_linked_list_node_reset(n3);

    /* 3. Build a list: n1 -> n2 -> n3 */
    aws_linked_list_push_back(&list, n1);
    aws_linked_list_push_back(&list, n2);
    aws_linked_list_push_back(&list, n3);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 4. Choose the middle node for removal */
    struct aws_linked_list_node *node = n2;

    /* 5. Save surrounding nodes and list state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list old_list = list;

    /* 6. Remove the node */
    aws_linked_list_remove(node);

    /* 7. Post‑conditions */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
    assert(!aws_linked_list_node_is_in_list(node));

    /* 8. List structure fields (sentinel links) remain unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 9. Non‑removed nodes retain correct links */
    assert(n1->prev == &list.head);
    assert(n1->next == n3);
    assert(n3->prev == n1);
    assert(n3->next == &list.tail);

    /* 10. List validity after removal */
    assert(aws_linked_list_is_valid(&list));
}
