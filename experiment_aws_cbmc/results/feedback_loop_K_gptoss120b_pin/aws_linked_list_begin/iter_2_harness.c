/*
 * Harness for aws_linked_list_begin
 * Verifies that the function returns the correct iterator and does not modify the list.
 */

#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_begin_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);
    aws_linked_list_init(list);

    /* Nondeterministically decide whether the list is empty */
    bool non_empty = __CPROVER_nondet_bool();

    if (non_empty) {
        /* Allocate a single node and link it as the sole element */
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        node->next = &list->tail;
        node->prev = &list->head;
        list->head.next = node;
        list->tail.prev = node;
    } else {
        /* Ensure the list is empty */
        list->head.next = &list->tail;
        list->tail.prev = &list->head;
    }

    /* Assume the list is valid according to the library's predicate */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Preserve a copy of the list for frame checking after it is fully constructed */
    struct aws_linked_list old_list = *list;

    /* Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_begin(list);

    /* Postcondition: return value is non‑NULL */
    assert(rval != NULL);

    /* Postcondition: correct element is returned */
    if (aws_linked_list_empty(list)) {
        assert(rval == &list->tail);
    } else {
        assert(rval->prev == &list->head);
    }

    /* Frame condition: the list structure is unchanged */
    assert(old_list.head.next == list->head.next);
    assert(old_list.head.prev == list->head.prev);
    assert(old_list.tail.next == list->tail.next);
    assert(old_list.tail.prev == list->tail.prev);

    /* If a node was allocated, its fields must remain unchanged */
    if (non_empty) {
        struct aws_linked_list_node *node = list->head.next;
        assert(node->next == &list->tail);
        assert(node->prev == &list->head);
    }
}
