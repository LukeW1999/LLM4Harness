#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_prev_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node = ensure_linked_list_node_is_allocated(NULL, allocator);
    __CPROVER_assume(node != NULL);

    aws_linked_list_push_back(&list, node);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Snapshot of relevant state */
    struct aws_linked_list_node *orig_prev = node->prev;
    struct aws_linked_list_node *orig_next = node->next;
    struct aws_linked_list_node *orig_head_next = list.head.next;
    struct aws_linked_list_node *orig_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* ASSERT_POSTCONDITIONS_HERE */
}
