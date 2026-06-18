#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness() {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate two nodes and add them to the list */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    struct aws_linked_list_node *node2 = malloc(sizeof(*node2));
    __CPROVER_assume(node1 != NULL);
    __CPROVER_assume(node2 != NULL);
    aws_linked_list_node_reset(node1);
    aws_linked_list_node_reset(node2);
    aws_linked_list_push_back(&list, node1);
    aws_linked_list_push_back(&list, node2);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Choose a node that is in the list (but not a sentinel) */
    struct aws_linked_list_node *node = nondet_bool() ? node1 : node2;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head);
    __CPROVER_assume(node != &list.tail);

    /* 4. Save old state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* 5. Call function under test */
    aws_linked_list_remove(node);

    /* 6. Post‑condition: the removed node is reset */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* 7. Post‑condition: surrounding nodes are linked together */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 8. Unchanged fields of the list structure */
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    /* 9. List validity invariant must hold */
    assert(aws_linked_list_is_valid(&list));
}
