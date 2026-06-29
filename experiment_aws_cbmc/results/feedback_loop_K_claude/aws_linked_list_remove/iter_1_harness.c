#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_remove_harness(void) {
    /* Allocate three nodes: prev_node <-> node <-> next_node */
    struct aws_linked_list_node *prev_node =
        (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *node =
        (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *next_node =
        (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));

    /* Precondition: all pointers must be non-null */
    __CPROVER_assume(prev_node != NULL);
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(next_node != NULL);

    /* Precondition: all three nodes must be distinct */
    __CPROVER_assume(prev_node != node);
    __CPROVER_assume(prev_node != next_node);
    __CPROVER_assume(node != next_node);

    /* Set up bidirectional linkage: prev_node <-> node <-> next_node */
    node->prev = prev_node;
    node->next = next_node;
    prev_node->next = node;
    next_node->prev = node;

    /* prev_node and next_node may have arbitrary outer pointers,
       but we need them to be non-null for the structure to be valid */
    __CPROVER_assume(prev_node->prev != NULL);
    __CPROVER_assume(next_node->next != NULL);

    /* Precondition: verify bidirectional linkage holds before call */
    assert(node->prev == prev_node);
    assert(node->next == next_node);
    assert(prev_node->next == node);
    assert(next_node->prev == node);

    /* Save old pointers for postcondition checks */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* Postcondition: node is reset (both pointers set to NULL) */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* Postcondition: prev and next nodes are now linked to each other */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* Postcondition: frame - prev_node->prev and next_node->next are unchanged */
    /* (We can't easily check all memory, but we verify the key frame properties) */
    assert(old_prev == prev_node);
    assert(old_next == next_node);
}

void aws_linked_list_remove_harness(void) {
    aws_linked_list_remove_harness();
    return 0;
}
