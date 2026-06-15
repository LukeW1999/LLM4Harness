#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate three nodes and build a list with at least one removable node */
    struct aws_linked_list_node *n1 = malloc(sizeof *n1);
    struct aws_linked_list_node *n2 = malloc(sizeof *n2);
    struct aws_linked_list_node *n3 = malloc(sizeof *n3);
    __CPROVER_assume(n1 && n2 && n3);

    aws_linked_list_node_reset(n1);
    aws_linked_list_node_reset(n2);
    aws_linked_list_node_reset(n3);

    aws_linked_list_push_back(&list, n1);
    aws_linked_list_push_back(&list, n2);
    aws_linked_list_push_back(&list, n3);

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Choose the middle node for removal */
    struct aws_linked_list_node *node = n2;

    /* 4. Save old state of surrounding nodes and the list */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list old_list = list;

    /* 5. Call the function under test */
    aws_linked_list_remove(node);

    /* 6. Post‑condition: the removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 7. Post‑condition: surrounding nodes are linked directly */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 8. Post‑condition: the node is no longer considered part of a list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* 9. Unchanged fields of the list structure (compare values, not addresses) */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 10. Unchanged fields of the non‑removed nodes */
    /* n1 is now the first element */
    assert(n1->prev == &list.head);
    assert(n1->next == n3);
    /* n3 is now the last element */
    assert(n3->prev == n1);
    assert(n3->next == &list.tail);

    /* 11. Validity invariant for the list after removal */
    assert(aws_linked_list_is_valid(&list));
}
