#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* CBMC harness for aws_linked_list_push_front */
void aws_linked_list_push_front_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);
    aws_linked_list_init(list);

    /* Optionally create an existing node already in the list */
    struct aws_linked_list_node *existing = NULL;
    if (__CPROVER_nondet_bool()) {
        existing = malloc(sizeof(*existing));
        __CPROVER_assume(existing != NULL);
        aws_linked_list_node_reset(existing);
        aws_linked_list_push_back(list, existing);
    }

    /* Allocate the node to be pushed to the front */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);

    /* Snapshot pre‑state of the list (head/tail anchors) */
    struct aws_linked_list old_list = *list;
    struct aws_linked_list_node *old_head_next = list->head.next;
    struct aws_linked_list_node *old_tail_prev = list->tail.prev;

    /* Call the function under test */
    aws_linked_list_push_front(list, node);

    /* ----- Post‑conditions ----- */

    /* 1. List validity */
    __CPROVER_assert(aws_linked_list_is_valid(list),
                     "aws_linked_list_is_valid(list) after push_front");

    /* 2. Node is now the first element */
    __CPROVER_assert(list->head.next == node,
                     "list->head.next == node after push_front");
    __CPROVER_assert(node->prev == &list->head,
                     "node->prev == &list->head after push_front");

    if (existing != NULL) {
        /* Non‑empty original list */
        __CPROVER_assert(node->next == existing,
                         "node->next points to previous first element");
        __CPROVER_assert(existing->prev == node,
                         "previous first element's prev points to new node");
    } else {
        /* Empty original list */
        __CPROVER_assert(node->next == &list->tail,
                         "node->next == &list->tail when list was empty");
        __CPROVER_assert(list->tail.prev == node,
                         "list->tail.prev == node when list was empty");
    }

    /* 3. Frame conditions (anchors unchanged) */
    __CPROVER_assert(old_list.head.prev == list->head.prev,
                     "list->head.prev unchanged");
    __CPROVER_assert(old_list.tail.next == list->tail.next,
                     "list->tail.next unchanged");

    /* Clean up */
    free(node);
    if (existing) {
        free(existing);
    }
    free(list);
    return 0;
}
