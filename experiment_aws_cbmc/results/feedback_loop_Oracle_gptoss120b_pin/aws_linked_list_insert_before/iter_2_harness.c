#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_insert_before_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate and reset two nodes to form an initial list */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    struct aws_linked_list_node *node2 = malloc(sizeof(*node2));
    __CPROVER_assume(node1 != NULL && node2 != NULL);
    aws_linked_list_node_reset(node1);
    aws_linked_list_node_reset(node2);

    /* Build the list: head <-> node1 <-> node2 <-> tail */
    aws_linked_list_push_back(&list, node1);
    aws_linked_list_push_back(&list, node2);

    /* Choose the node before which we will insert */
    struct aws_linked_list_node *before = node2;

    /* Allocate and reset the node to be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);

    /* Snapshot relevant state before the call */
    struct aws_linked_list_node *old_prev = before->prev;               /* should be node1 */
    struct aws_linked_list_node old_node1 = *node1;
    struct aws_linked_list_node old_node2 = *node2;
    struct aws_linked_list_node old_before = *before;
    struct aws_linked_list_node old_oldprev = *old_prev;

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* Post‑condition: the list remains a valid doubly‑linked list */
    __CPROVER_assert(aws_linked_list_is_valid(&list), "list is valid");
    __CPROVER_assert(aws_linked_list_is_valid_deep(&list), "list is deep valid");

    /* Post‑condition: the inserted node is correctly linked */
    __CPROVER_assert(to_add->next == before, "to_add->next points to before");
    __CPROVER_assert(to_add->prev == old_prev, "to_add->prev points to old_prev");
    __CPROVER_assert(before->prev == to_add, "before->prev points to to_add");
    __CPROVER_assert(old_prev->next == to_add, "old_prev->next points to to_add");

    /* Frame conditions: nodes retain appropriate connections */
    __CPROVER_assert(node1->prev == old_node1.prev, "node1->prev unchanged");
    __CPROVER_assert(node1->next == to_add, "node1->next now points to to_add");
    __CPROVER_assert(node2->next == old_node2.next, "node2->next unchanged");
    __CPROVER_assert(before->next == old_before.next, "before->next unchanged");
    __CPROVER_assert(old_prev->prev == old_oldprev.prev, "old_prev->prev unchanged");

    /* Clean up */
    free(node1);
    free(node2);
    free(to_add);
}
