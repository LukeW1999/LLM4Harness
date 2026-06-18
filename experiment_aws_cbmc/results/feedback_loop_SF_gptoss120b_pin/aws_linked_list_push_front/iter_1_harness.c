#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness(void) {
    /* allocator (not used directly but kept for consistency) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* symbolic linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* symbolic node to be pushed */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);

    /* snapshot of list state before the call */
    struct aws_linked_list_node *orig_head_next = list.head.next;
    struct aws_linked_list_node *orig_tail_prev = list.tail.prev;

    /* call under verification */
    aws_linked_list_push_front(&list, node);

    /* postcondition assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == node);
    assert(node->prev == &list.head);
    assert(node->next == orig_head_next);
    if (orig_head_next != &list.tail) {
        assert(orig_head_next->prev == node);
        assert(list.tail.prev == orig_tail_prev);
    } else {
        assert(list.tail.prev == node);
    }
}
