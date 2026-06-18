#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    /* Allocate list and node */
    struct aws_linked_list *list = malloc(sizeof *list);
    __CPROVER_assume(list != NULL);
    struct aws_linked_list_node *node = malloc(sizeof *node);
    __CPROVER_assume(node != NULL);

    /* Initialize list as a valid (possibly empty) list.
       For simplicity we start with an empty list. */
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;

    /* Ensure the list satisfies the validity predicate */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Remember the old last element (the node before tail) */
    struct aws_linked_list_node *old_last = list->tail.prev;

    /* Snapshot memory for frame condition checks */
    struct aws_linked_list list_snapshot = *list;
    struct aws_linked_list_node node_snapshot = *node;
    struct aws_linked_list_node old_last_snapshot;
    if (old_last != NULL) {
        old_last_snapshot = *old_last;
    }

    /* Call the function under test */
    aws_linked_list_push_back(list, node);

    /* ----- Postcondition checks ----- */
    __CPROVER_assert(aws_linked_list_is_valid(list),
                     "list remains a valid linked list after push_back");
    __CPROVER_assert(list->tail.prev == node,
                     "node is the new last element (tail.prev == node)");
    __CPROVER_assert(node->next == &list->tail,
                     "node->next points to the tail sentinel");
    __CPROVER_assert(node->prev == old_last,
                     "node->prev points to the previous last element");
    __CPROVER_assert(old_last->next == node,
                     "previous last element now points to the new node");

    /* ----- Frame condition checks (memory not modified beyond contract) ----- */
    /* Head sentinel fields */
    __CPROVER_assert(list->head.prev == list_snapshot.head.prev,
                     "head.prev unchanged");
    if (old_last == &list->head) {
        /* Empty list case: head.next should now point to the new node */
        __CPROVER_assert(list->head.next == node,
                         "head.next updated to new node for empty list");
    } else {
        __CPROVER_assert(list->head.next == list_snapshot.head.next,
                         "head.next unchanged for non‑empty list");
    }

    /* Tail sentinel fields */
    __CPROVER_assert(list->tail.next == list_snapshot.tail.next,
                     "tail.next unchanged");

    /* Ensure that no other fields of the old last node were altered */
    if (old_last != NULL) {
        __CPROVER_assert(old_last->prev == old_last_snapshot.prev,
                         "old_last.prev unchanged");
        __CPROVER_assert(old_last->next == node,
                         "old_last.next correctly updated to new node");
    }

    /* Node fields other than next/prev are unchanged (node has no other fields) */
    (void)node_snapshot; /* suppress unused variable warning */

    return 0;
}
