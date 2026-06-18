#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness() {
    /* Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure the list is non‑empty so we have a real node (not a sentinel) */
    __CPROVER_assume(list.head.next != &list.tail);

    /* Pick a node that is part of the list */
    struct aws_linked_list_node *node = list.head.next;

    /* Save old state of the node */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    /* Save old state of the list for unchanged‑field checks */
    struct aws_linked_list old = list;

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
