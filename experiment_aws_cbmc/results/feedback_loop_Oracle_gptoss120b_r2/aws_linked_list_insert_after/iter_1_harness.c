#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    /* Initialize list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Create first node and add to list */
    struct aws_linked_list_node *node1 = malloc(sizeof *node1);
    __CPROVER_assume(node1 != NULL);
    aws_linked_list_node_reset(node1);
    aws_linked_list_push_back(&list, node1);

    /* Optionally create a second node */
    bool have_node2 = nondet_bool();
    struct aws_linked_list_node *node2 = NULL;
    if (have_node2) {
        node2 = malloc(sizeof *node2);
        __CPROVER_assume(node2 != NULL);
        aws_linked_list_node_reset(node2);
        aws_linked_list_push_back(&list, node2);
    }

    /* Choose a valid 'after' node (could be head sentinel, node1, or node2) */
    struct aws_linked_list_node *after;
    {
        int choice = nondet_int();
        __CPROVER_assume(choice >= 0 && choice <= 2);
        if (choice == 0) {
            after = &list.head;
        } else if (choice == 1) {
            after = node1;
        } else {
            __CPROVER_assume(node2 != NULL);
            after = node2;
        }
    }

    /* Snapshot of the node that follows 'after' before insertion */
    struct aws_linked_list_node *old_next = after->next;

    /* Snapshot of next/prev of existing nodes for frame condition checks */
    struct aws_linked_list_node *node1_next_pre = NULL, *node1_prev_pre = NULL;
    struct aws_linked_list_node *node2_next_pre = NULL, *node2_prev_pre = NULL;
    if (node1 != NULL) {
        node1_next_pre = node1->next;
        node1_prev_pre = node1->prev;
    }
    if (node2 != NULL) {
        node2_next_pre = node2->next;
        node2_prev_pre = node2->prev;
    }

    /* Create node to add */
    struct aws_linked_list_node *to_add = malloc(sizeof *to_add);
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(after->next != NULL); /* after must have a successor */

    /* Call function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postconditions */

    /* List must remain valid */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* to_add connections */
    assert(to_add->prev == after);
    assert(to_add->next == old_next);

    /* after connections */
    assert(after->next == to_add);

    /* old_next connections (if old_next is not the tail sentinel) */
    if (old_next != &list.tail) {
        assert(old_next->prev == to_add);
    }

    /* Frame condition: other nodes unchanged */
    if (node1 != NULL && node1 != after && node1 != to_add && node1 != old_next) {
        assert(node1->next == node1_next_pre);
        assert(node1->prev == node1_prev_pre);
    }
    if (node2 != NULL && node2 != after && node2 != to_add && node2 != old_next) {
        assert(node2->next == node2_next_pre);
        assert(node2->prev == node2_prev_pre);
    }

    return 0;
}
