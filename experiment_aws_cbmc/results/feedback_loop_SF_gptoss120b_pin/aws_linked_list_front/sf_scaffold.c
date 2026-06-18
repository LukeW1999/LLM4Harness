#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate and initialize a linked list */
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);
    aws_linked_list_init(list);

    /* Nondeterministically add a bounded number of nodes to the list */
    size_t max_nodes = nondet_uint();
    __CPROVER_assume(max_nodes <= 5);
    for (size_t i = 0; i < max_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(list, node);
    }

    /* Precondition: the list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Snapshot of input state */
    struct aws_linked_list_node *orig_head_next = list->head.next;
    size_t orig_len = 0;
    for (struct aws_linked_list_node *cur = list->head.next; cur != &list->tail; cur = cur->next) {
        ++orig_len;
    }

    /* Call the function under verification */
    struct aws_linked_list_node *result = aws_linked_list_front(list);

    /* ASSERT_POSTCONDITIONS_HERE */
}
