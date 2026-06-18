#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create the node to insert before (must be a valid node in the list) */
    /* We'll use a node that is already in the list - pick head.next as 'before'
       if list is non-empty, or use tail (which is always valid as a sentinel) */
    struct aws_linked_list_node *before;
    struct aws_linked_list_node *to_add;

    /* Allocate to_add as a fresh node */
    to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* 'before' must be a node in the list with a valid prev pointer.
       We use the tail sentinel as 'before' to insert at the back,
       or nondet choose between head.next (if non-empty) and tail.
       For simplicity, use tail as 'before' — always valid. */
    bool use_tail = nondet_bool();
    if (use_tail || aws_linked_list_empty(&list)) {
        before = &list.tail;
    } else {
        /* Use head.next which is a real node */
        before = list.head.next;
    }

    /* Preconditions: before must have a valid prev pointer */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    /* The bidirectional link must hold: before->prev->next == before */
    __CPROVER_assume(before->prev->next == before);

    /* Save state before the call */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* Postconditions: to_add is inserted immediately before 'before' */

    /* 1. to_add->next must point to before */
    assert(to_add->next == before);

    /* 2. to_add->prev must point to old before->prev */
    assert(to_add->prev == old_before_prev);

    /* 3. before->prev must now point to to_add */
    assert(before->prev == to_add);

    /* 4. old_before_prev->next must now point to to_add */
    assert(old_before_prev->next == to_add);

    /* 5. before->next is unchanged */
    assert(before->next == old_before_next);

    /* 6. Bidirectional link: to_add->next->prev == to_add */
    assert(to_add->next->prev == to_add);

    /* 7. Bidirectional link: to_add->prev->next == to_add */
    assert(to_add->prev->next == to_add);

    /* 8. The list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
