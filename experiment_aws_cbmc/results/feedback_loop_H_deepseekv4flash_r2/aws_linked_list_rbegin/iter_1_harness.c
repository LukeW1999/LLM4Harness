#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness() {
    /* Data structure */
    struct aws_linked_list list;

    /* Non-deterministically allocate and initialize a valid linked list */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list)); /* rbegin requires non-empty list */

    /* Save old state (const pointer, no modification, but for completeness) */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call function */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* Postcondition: list is unchanged */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Postcondition: list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition: return value equals tail.prev (the definition) */
    assert(rval == list.tail.prev);

    /* Postcondition: returned node is the last element */
    /* -> its next pointer points to the tail sentinel */
    assert(rval->next == &list.tail);
    /* -> its prev pointer points to the previous node or head (if only one element) */
    /* We can check that the node is valid: */
    assert(aws_linked_list_node_next_is_valid(rval));
    assert(aws_linked_list_node_prev_is_valid(rval));
}
