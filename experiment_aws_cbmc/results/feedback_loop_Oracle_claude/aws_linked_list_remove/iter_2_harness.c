#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_remove_harness(void) {
    /* Allocate a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate nodes to add to the list */
    struct aws_linked_list_node *node_before = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *node_to_remove = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *node_after = malloc(sizeof(struct aws_linked_list_node));

    __CPROVER_assume(node_before != NULL);
    __CPROVER_assume(node_to_remove != NULL);
    __CPROVER_assume(node_after != NULL);

    /* Build a list: head <-> node_before <-> node_to_remove <-> node_after <-> tail */
    aws_linked_list_push_back(&list, node_before);
    aws_linked_list_push_back(&list, node_to_remove);
    aws_linked_list_push_back(&list, node_after);

    /* Verify the list is valid before removal */
    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));

    /* Save pointers to neighbors before removal */
    struct aws_linked_list_node *prev_node = node_to_remove->prev;
    struct aws_linked_list_node *next_node = node_to_remove->next;

    /* Call the function under test */
    aws_linked_list_remove(node_to_remove);

    /* Postcondition 2: The neighbors are now linked to each other */
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);

    /* Postcondition 3: The list is still valid after removal */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 4: The list still has two elements (node_before and node_after) */
    assert(!aws_linked_list_empty(&list));

    /* Postcondition 5: node_before and node_after are still in the list */
    assert(aws_linked_list_begin(&list) == node_before);
    assert(node_before->next == node_after);
    assert(node_after->prev == node_before);
    assert(node_after->next == aws_linked_list_end(&list));
}
