#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_linked_list_next_harness(void) {
    /* Allocate the primary node */
    struct aws_linked_list_node *node = malloc(sizeof *node);
    __CPROVER_assume(node != NULL);

    /* Allocate a possible next node */
    struct aws_linked_list_node *next_node = malloc(sizeof *next_node);
    __CPROVER_assume(next_node != NULL);

    /* Nondeterministically decide whether the list is linked */
    bool link = __CPROVER_nondet_bool();

    if (link) {
        node->next = next_node;
        next_node->prev = node;
    } else {
        node->next = NULL;
    }

    /* The prev pointer of the primary node is irrelevant for aws_linked_list_next */
    node->prev = NULL;

    /* Ensure the possible next node is otherwise well‑formed */
    next_node->next = NULL;
    if (!link) {
        next_node->prev = NULL;
    }

    /* Assume the precondition required by aws_linked_list_next */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    /* Save copies for frame condition checks */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list_node old_next_node = *next_node;

    /* Call the function under verification */
    const struct aws_linked_list_node *ret = aws_linked_list_next(node);

    /* Post‑condition 1: return value equals the stored next pointer */
    assert(ret == old_node.next);

    /* Post‑condition 2: if a next node exists, the bidirectional link is valid */
    if (ret != NULL) {
        assert(ret->prev == node);
    }

    /* Frame conditions – the primary node is unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* Frame condition – the possible next node is unchanged */
    assert(next_node->next == old_next_node.next);
    assert(next_node->prev == old_next_node.prev);

    /* Clean up */
    free(next_node);
    free(node);
}
