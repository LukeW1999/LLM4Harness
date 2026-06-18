#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_remove_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* allocate and initialize a linked list */
    struct aws_linked_list *list = (struct aws_linked_list *)malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);
    aws_linked_list_init(list);

    /* allocate a node that will be removed */
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);

    /* insert the node into the list (list now non‑empty) */
    aws_linked_list_push_back(list, node);

    /* optional: nondeterministically add additional nodes to the list */
    {
        size_t extra_nodes = nondet_size_t();
        __CPROVER_assume(extra_nodes <= 5); /* bound to keep verification tractable */
        for (size_t i = 0; i < extra_nodes; ++i) {
            struct aws_linked_list_node *n = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
            __CPROVER_assume(n != NULL);
            aws_linked_list_node_reset(n);
            aws_linked_list_push_back(list, n);
        }
    }

    /* preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(list));
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /* snapshot of relevant state before the call */
    size_t original_len = 0;
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        original_len++;
    }
    struct aws_linked_list_node *orig_prev = node->prev;
    struct aws_linked_list_node *orig_next = node->next;

    /* call under verification */
    aws_linked_list_remove(node);

    /* ASSERT_POSTCONDITIONS_HERE */
}
