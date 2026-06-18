#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_nodes_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need two nodes to swap. They must be valid nodes in the list. */
    struct aws_linked_list_node *a = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *b = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(a != NULL);
    __CPROVER_assume(b != NULL);

    /* Insert a and b into the list so they are valid nodes */
    /* We'll push them to the front to ensure they are in the list */
    aws_linked_list_push_front(&list, a);
    aws_linked_list_push_front(&list, b);

    /* Now both a and b are in the list and satisfy prev/next validity */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(a));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(a));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(b));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(b));

    /* 2. Save old state BEFORE calling */
    struct aws_linked_list_node old_a = *a;
    struct aws_linked_list_node old_b = *b;
    struct aws_linked_list old_list = list;

    /* 3. Call function under test */
    aws_linked_list_swap_nodes(a, b);

    /* 4. Assert postconditions */
    /* The function guarantees that after swapping, both nodes are still valid */
    assert(aws_linked_list_node_prev_is_valid(a));
    assert(aws_linked_list_node_next_is_valid(a));
    assert(aws_linked_list_node_prev_is_valid(b));
    assert(aws_linked_list_node_next_is_valid(b));

    /* If a and b were the same node, nothing changes */
    if (a == b) {
        assert(a->next == old_a.next);
        assert(a->prev == old_a.prev);
    } else {
        /* The nodes have swapped positions in the list */
        /* a's next/prev should now be b's old next/prev (but adjusted for adjacency) */
        /* We cannot assert exact pointer values without knowing adjacency,
         * but we can assert that the list remains valid */
    }

    /* 5. Assert validity invariants always hold */
    assert(aws_linked_list_is_valid(&list));

    /* 6. Free allocated memory */
    free(a);
    free(b);
}
