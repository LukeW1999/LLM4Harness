#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    /* Allocate three nodes: 'after', 'next_node' (originally after->next), and 'to_add' */
    struct aws_linked_list_node *after    = malloc(sizeof(*after));
    struct aws_linked_list_node *next_node = malloc(sizeof(*next_node));
    struct aws_linked_list_node *to_add   = malloc(sizeof(*to_add));

    /* Precondition: all pointers must be non-null */
    __CPROVER_assume(after    != NULL);
    __CPROVER_assume(next_node != NULL);
    __CPROVER_assume(to_add   != NULL);

    /* Precondition: after and next_node are distinct */
    __CPROVER_assume(after != next_node);

    /* Precondition: to_add is distinct from after and next_node */
    __CPROVER_assume(to_add != after);
    __CPROVER_assume(to_add != next_node);

    /* Set up a locally consistent doubly-linked segment: after <-> next_node */
    after->next     = next_node;
    after->prev     = NULL;   /* not relevant to this operation */
    next_node->prev = after;
    next_node->next = NULL;   /* not relevant to this operation */

    /* to_add's pointers are unconstrained before the call */

    /* Save old values for postcondition checks */
    struct aws_linked_list_node *old_after_next = after->next; /* == next_node */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_next_node_next = next_node->next;

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postcondition: to_add->prev points to after */
    assert(to_add->prev == after);

    /* Postcondition: to_add->next points to the old after->next (next_node) */
    assert(to_add->next == old_after_next);

    /* Postcondition: after->next now points to to_add */
    assert(after->next == to_add);

    /* Postcondition: old after->next (next_node)->prev now points to to_add */
    assert(old_after_next->prev == to_add);

    /* Frame condition: after->prev was not modified */
    assert(after->prev == old_after_prev);

    /* Frame condition: next_node->next was not modified */
    assert(next_node->next == old_next_node_next);

    /* Structural integrity: the chain after <-> to_add <-> next_node is consistent */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == next_node);
    assert(next_node->prev == to_add);
}

void aws_linked_list_insert_after_harness(void) {
    aws_linked_list_insert_after_harness();
    return 0;
}
