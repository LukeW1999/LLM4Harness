#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create an existing node and link it into the list */
    struct aws_linked_list_node *existing = malloc(sizeof(*existing));
    __CPROVER_assume(existing != NULL);
    existing->next = &list.tail;
    existing->prev = &list.head;
    list.head.next = existing;
    list.tail.prev = existing;
    list.head.prev = NULL;
    list.tail.next = NULL;

    /* Allocate the node to be added and reset it */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL);

    /* Choose a valid 'before' node: either the existing node or the tail sentinel */
    struct aws_linked_list_node *before;
    if (nondet_bool()) {
        before = existing;
    } else {
        before = &list.tail;
    }
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL); /* must have a predecessor */

    /* Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* Post‑conditions: linkage around the inserted node */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    assert(old_before_prev->next == to_add);
    assert(before->next == old_before_next); /* before's next is unchanged */

    /* Additional checks depending on which node was used as 'before' */
    if (before == &list.tail) {
        assert(list.tail.prev == to_add);
    } else {
        /* before == existing, which means insertion at the front */
        assert(list.head.next == to_add);
    }

    /* Unchanged sentinel fields */
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    /* The inserted node should now be considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* Global validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
