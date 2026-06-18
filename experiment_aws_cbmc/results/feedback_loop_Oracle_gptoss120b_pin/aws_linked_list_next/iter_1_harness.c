#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 5

/* CBMC nondeterministic size_t */
size_t nondet_size_t(void);

void aws_linked_list_next_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* nondet number of nodes (0..MAX_NODES) */
    size_t num_nodes = nondet_size_t();
    __CPROVER_assume(num_nodes <= MAX_NODES);

    struct aws_linked_list_node *nodes[MAX_NODES];
    for (size_t i = 0; i < num_nodes; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* pick a candidate node: head, tail, or one of the allocated nodes */
    size_t choice = nondet_size_t();
    __CPROVER_assume(choice <= num_nodes + 1);
    struct aws_linked_list_node *candidate;
    if (choice == 0) {
        candidate = &list.head;
    } else if (choice == 1) {
        candidate = &list.tail;
    } else {
        candidate = nodes[choice - 2];
    }

    /* Save pre‑state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_next = candidate->next;

    /* Call the function under test */
    struct aws_linked_list_node *ret = aws_linked_list_next(candidate);

    /* Post‑conditions */

    /* The list must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* Frame condition: the list structure itself is unchanged */
    assert(memcmp(&list, &old_list, sizeof(list)) == 0);

    /* The candidate's next pointer is unchanged */
    assert(candidate->next == old_next);

    /* Return value equals the saved next pointer */
    assert(ret == old_next);

    /* If a node is returned, its prev pointer must point back to the candidate */
    if (ret != NULL) {
        assert(ret->prev == candidate);
    }

    /* Clean up allocated nodes */
    for (size_t i = 0; i < num_nodes; ++i) {
        free(nodes[i]);
    }

    return 0;
}
