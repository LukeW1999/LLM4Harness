#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Wrapper to create a framing region around the node under test */
struct wrapper {
    struct aws_linked_list_node guard_before;
    struct aws_linked_list_node node;
    struct aws_linked_list_node guard_after;
};

void aws_linked_list_node_reset_harness(void) {
    struct wrapper *w = malloc(sizeof(struct wrapper));
    __CPROVER_assume(w != NULL);

    /* Nondeterministically initialise all fields */
    w->guard_before.next = (struct aws_linked_list_node *)malloc(1);
    w->guard_before.prev = (struct aws_linked_list_node *)malloc(1);
    w->node.next          = (struct aws_linked_list_node *)malloc(1);
    w->node.prev          = (struct aws_linked_list_node *)malloc(1);
    w->guard_after.next  = (struct aws_linked_list_node *)malloc(1);
    w->guard_after.prev  = (struct aws_linked_list_node *)malloc(1);

    /* Preserve the framing region for later comparison */
    struct aws_linked_list_node guard_before_old = w->guard_before;
    struct aws_linked_list_node guard_after_old  = w->guard_after;

    /* Call the function under verification */
    aws_linked_list_node_reset(&w->node);

    /* 1. Post‑condition: the node must be zeroed (both pointers NULL) */
    assert(w->node.next == NULL);
    assert(w->node.prev == NULL);

    /* 2. Frame condition: surrounding memory must be unchanged */
    assert(w->guard_before.next == guard_before_old.next);
    assert(w->guard_before.prev == guard_before_old.prev);
    assert(w->guard_after.next  == guard_after_old.next);
    assert(w->guard_after.prev  == guard_after_old.prev);

    /* Clean‑up (not required for verification but keeps tools happy) */
    free(w->guard_before.next);
    free(w->guard_before.prev);
    free(w->node.next);
    free(w->node.prev);
    free(w->guard_after.next);
    free(w->guard_after.prev);
    free(w);

    return 0;
}
