#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    /* Allocate two nodes non-deterministically */
    struct aws_linked_list_node *after = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *after_next = malloc(sizeof(struct aws_linked_list_node));

    /* Ensure allocations succeeded */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(to_add != NULL);
    __CPROVER_assume(after_next != NULL);

    /* Ensure all nodes are distinct */
    __CPROVER_assume(after != to_add);
    __CPROVER_assume(after != after_next);
    __CPROVER_assume(to_add != after_next);

    /* Set up a valid doubly-linked connection between after and after_next */
    after->next = after_next;
    after_next->prev = after;

    /* after_next->next can be anything valid (non-null for safety) */
    struct aws_linked_list_node *after_next_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_next_next != NULL);
    __CPROVER_assume(after_next_next != after);
    __CPROVER_assume(after_next_next != to_add);
    __CPROVER_assume(after_next_next != after_next);
    after_next->next = after_next_next;
    after_next_next->prev = after_next;

    /* after->prev can be anything */
    struct aws_linked_list_node *after_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_prev != NULL);
    __CPROVER_assume(after_prev != after);
    __CPROVER_assume(after_prev != to_add);
    __CPROVER_assume(after_prev != after_next);
    __CPROVER_assume(after_prev != after_next_next);
    after->prev = after_prev;
    after_prev->next = after;

    /* Save old values for frame condition checks */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next = after->next; /* = after_next */
    struct aws_linked_list_node *old_after_next_next = after_next->next;

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postcondition 1: to_add->prev points to after */
    assert(to_add->prev == after);

    /* Postcondition 2: to_add->next points to old after->next (after_next) */
    assert(to_add->next == old_after_next);

    /* Postcondition 3: after->next points to to_add */
    assert(after->next == to_add);

    /* Postcondition 4: old after->next->prev points to to_add */
    assert(old_after_next->prev == to_add);

    /* Postcondition 5: Bidirectional linkage - after <-> to_add */
    assert(after->next == to_add);
    assert(to_add->prev == after);

    /* Postcondition 6: Bidirectional linkage - to_add <-> after_next */
    assert(to_add->next == after_next);
    assert(after_next->prev == to_add);

    /* Frame condition 1: after->prev is unchanged */
    assert(after->prev == old_after_prev);

    /* Frame condition 2: after_next->next is unchanged */
    assert(after_next->next == old_after_next_next);

    /* Frame condition 3: after_prev->next is unchanged (still points to after) */
    assert(after_prev->next == after);

    /* Frame condition 4: after_next_next->prev is unchanged */
    assert(after_next_next->prev == after_next);

    /* Validity: the chain after -> to_add -> after_next is consistent */
    assert(after->next->next == after_next);
    assert(after_next->prev->prev == after);
}

void aws_linked_list_insert_after_harness(void) {
    aws_linked_list_insert_after_harness();
    return 0;
}
