#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_next_harness() {
    /* Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state of the list for unchanged‑field checks */
    struct aws_linked_list old = list;

    /* Allocate a node that will be used as input */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Non‑deterministically decide whether the node's next pointer is NULL
       or points to another allocated node. */
    if (nondet_bool()) {
        node->next = NULL;
    } else {
        node->next = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node->next != NULL);
        /* Link the second node back to the first to keep it a well‑formed
           two‑node fragment (not required for the function, but harmless). */
        node->next->prev = node;
        node->next->next = NULL;
    }
    /* The prev pointer is irrelevant for the function; set it nondet. */
    node->prev = NULL;

    /* Save old state of the node */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* Post‑conditions */
    assert(result == old_next);                     /* returned pointer unchanged */
    assert(node->next == old_next);                 /* node->next unchanged */
    assert(node->prev == old_prev);                 /* node->prev unchanged */

    /* List must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
