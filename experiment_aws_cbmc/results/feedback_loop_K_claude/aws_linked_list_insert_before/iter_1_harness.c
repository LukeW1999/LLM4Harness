#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    /* Allocate three nodes: prev_node <-> before, and to_add to be inserted */
    struct aws_linked_list_node *before = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));

    /* Preconditions: all pointers must be non-null */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(prev_node != NULL);
    __CPROVER_assume(to_add != NULL);

    /* Precondition: before and to_add are distinct nodes */
    __CPROVER_assume(before != to_add);

    /* Precondition: prev_node is distinct from before and to_add */
    __CPROVER_assume(prev_node != before);
    __CPROVER_assume(prev_node != to_add);

    /* Set up a locally consistent doubly-linked structure:
     * prev_node <-> before
     * This satisfies: before->prev == prev_node && prev_node->next == before
     */
    before->prev = prev_node;
    prev_node->next = before;

    /* Give before->next and prev_node->prev some valid (non-null) values
     * to represent the rest of the list (we don't care about them for this proof) */
    struct aws_linked_list_node *after_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_node != NULL);
    __CPROVER_assume(after_node != before);
    __CPROVER_assume(after_node != prev_node);
    __CPROVER_assume(after_node != to_add);
    before->next = after_node;

    struct aws_linked_list_node *before_prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(before_prev_node != NULL);
    __CPROVER_assume(before_prev_node != before);
    __CPROVER_assume(before_prev_node != prev_node);
    __CPROVER_assume(before_prev_node != to_add);
    __CPROVER_assume(before_prev_node != after_node);
    prev_node->prev = before_prev_node;

    /* Save old values for postcondition checks */
    struct aws_linked_list_node *old_before_prev = before->prev; /* == prev_node */
    struct aws_linked_list_node *old_before_next = before->next; /* == after_node */
    struct aws_linked_list_node *old_prev_node_prev = prev_node->prev;

    /* Precondition: local consistency check */
    __CPROVER_assume(before->prev->next == before);

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* Postconditions */

    /* to_add->next must point to before */
    assert(to_add->next == before);

    /* to_add->prev must point to old before->prev (prev_node) */
    assert(to_add->prev == old_before_prev);

    /* before->prev must now point to to_add */
    assert(before->prev == to_add);

    /* old before->prev (prev_node)->next must now point to to_add */
    assert(old_before_prev->next == to_add);

    /* Frame conditions: before->next is unchanged */
    assert(before->next == old_before_next);

    /* Frame conditions: prev_node->prev is unchanged */
    assert(prev_node->prev == old_prev_node_prev);

    /* Local doubly-linked consistency around to_add */
    assert(to_add->next->prev == to_add);  /* before->prev == to_add */
    assert(to_add->prev->next == to_add);  /* prev_node->next == to_add */
}

void aws_linked_list_insert_before_harness(void) {
    aws_linked_list_insert_before_harness();
    return 0;
}
