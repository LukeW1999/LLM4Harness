#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* Harness for aws_linked_list_prev */
void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is part of the list, but not the head sentinel */
    const struct aws_linked_list_node *node;
    if (aws_linked_list_empty(&list)) {
        /* Empty list: only tail sentinel is selectable */
        node = &list.tail;
    } else {
        /* Non‑empty list: may pick the first real node or the tail sentinel */
        const struct aws_linked_list_node *first = list.head.next; /* first real node */
        node = nondet_bool() ? first : &list.tail;
    }
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head);          /* precondition: node->prev must be valid */
    __CPROVER_assume(node->prev != NULL);          /* ensures aws_linked_list_node_prev_is_valid */

    /* 3. Save old state of the list */
    struct aws_linked_list old = list;

    /* 4. Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Post‑conditions */

    /* 5.1. Return value equals node->prev */
    assert(result == node->prev);

    /* 5.2. The list structure itself must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 5.3. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
