#include <assert.h>
#include <stdlib.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_prev_harness(void) {
    /* 1. Initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node and insert it into the list */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    node->next = NULL;
    node->prev = NULL;
    aws_linked_list_push_back(&list, node);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Save copies of the relevant state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list_node *old_prev = node->prev;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Post‑conditions */
    assert(result == old_prev);
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
