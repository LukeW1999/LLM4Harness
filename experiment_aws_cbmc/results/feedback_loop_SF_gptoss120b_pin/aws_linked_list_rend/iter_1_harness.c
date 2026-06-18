#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_rend_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* Initialize the list */
    aws_linked_list_init(list);

    /* Nondeterministically add a bounded number of nodes */
    unsigned int max_nodes = nondet_uint();
    __CPROVER_assume(max_nodes <= 5);
    for (unsigned int i = 0; i < max_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(list, node);
    }

    /* Precondition: the list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Snapshot of the input state */
    struct aws_linked_list list_snapshot = *list;
    const struct aws_linked_list_node *orig_head_next = list->head.next;
    const struct aws_linked_list_node *orig_tail_prev = list->tail.prev;

    /* Call the function under verification */
    const struct aws_linked_list_node *r = aws_linked_list_rend(list);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* Return value must be the head sentinel */
    assert(r == &list->head);

    /* The list pointer must remain non‑NULL */
    assert(list != NULL);

    /* The list must still be valid after the call */
    assert(aws_linked_list_is_valid(list));

    /* Frame conditions: the list structure must be unchanged */
    assert(list->head.next == list_snapshot.head.next);
    assert(list->head.prev == list_snapshot.head.prev);
    assert(list->tail.next == list_snapshot.tail.next);
    assert(list->tail.prev == list_snapshot.tail.prev);

    /* The specific next/prev pointers captured before the call must be unchanged */
    assert(list->head.next == orig_head_next);
    assert(list->tail.prev == orig_tail_prev);
}
