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

    /* allocate two nodes and push them onto the list */
    struct aws_linked_list_node *node1 = ensure_linked_list_node_is_allocated(NULL, allocator);
    __CPROVER_assume(node1 != NULL);
    aws_linked_list_push_back(&list, node1);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node2 = ensure_linked_list_node_is_allocated(NULL, allocator);
    __CPROVER_assume(node2 != NULL);
    aws_linked_list_push_back(&list, node2);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Snapshot of relevant state before the call */
    struct aws_linked_list_node *orig_prev = node2->prev;
    struct aws_linked_list_node *orig_next = node2->next;
    struct aws_linked_list_node *orig_head_next = list.head.next;
    struct aws_linked_list_node *orig_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node2);

    /* Post‑conditions */
    assert(result == orig_prev);
    assert(node2->prev == orig_prev);
    assert(node2->next == orig_next);
    assert(list.head.next == orig_head_next);
    assert(list.tail.prev == orig_tail_prev);
    assert(aws_linked_list_is_valid(&list));
}
