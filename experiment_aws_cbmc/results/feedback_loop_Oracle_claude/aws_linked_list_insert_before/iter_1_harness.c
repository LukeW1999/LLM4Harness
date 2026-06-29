#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    /* Allocate a list and initialize it */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate nodes to insert */
    struct aws_linked_list_node *node_a = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *node_b = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *node_to_add = malloc(sizeof(struct aws_linked_list_node));

    if (!node_a || !node_b || !node_to_add) {
        return;
    }

    /* Push node_a and node_b into the list so we have a valid list with nodes */
    aws_linked_list_push_back(&list, node_a);
    aws_linked_list_push_back(&list, node_b);

    /* Preconditions: list must be valid before insertion */
    assert(aws_linked_list_is_valid(&list));

    /* Choose a node to insert before - nondeterministically pick one of:
     * - node_a (first element)
     * - node_b (second element)
     * - &list.tail (end sentinel)
     */
    struct aws_linked_list_node *before;
    int choice;
    __CPROVER_assume(choice >= 0 && choice <= 2);
    if (choice == 0) {
        before = node_a;
    } else if (choice == 1) {
        before = node_b;
    } else {
        before = &list.tail;
    }

    /* Save state before insertion for frame condition checks */
    struct aws_linked_list_node *before_prev_before = before->prev;
    struct aws_linked_list_node *before_next_before = before->next;

    /* Call the function under test */
    aws_linked_list_insert_before(before, node_to_add);

    /* Postcondition 1: to_add->next must point to before */
    assert(node_to_add->next == before);

    /* Postcondition 2: to_add->prev must point to before's original prev */
    assert(node_to_add->prev == before_prev_before);

    /* Postcondition 3: before->prev must now point to to_add */
    assert(before->prev == node_to_add);

    /* Postcondition 4: the original prev of before must now point to to_add */
    assert(before_prev_before->next == node_to_add);

    /* Postcondition 5: before->next is unchanged (frame condition) */
    assert(before->next == before_next_before);

    /* Postcondition 6: the list is still valid after insertion */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 7: the list is not empty */
    assert(!aws_linked_list_empty(&list));

    /* Postcondition 8: bidirectional linkage around to_add is correct */
    assert(node_to_add->next->prev == node_to_add);
    assert(node_to_add->prev->next == node_to_add);

    /* Postcondition 9: deep validity check */
    assert(aws_linked_list_is_valid_deep(&list));

    free(node_a);
    free(node_b);
    free(node_to_add);
}

void aws_linked_list_insert_before_harness(void) {
    aws_linked_list_insert_before_harness();
    return 0;
}
