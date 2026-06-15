#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 5

void aws_linked_list_remove_harness() {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_NODES);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Populate the list with a nondeterministic number of nodes */
    size_t num_nodes = nondet_uint32_t() % (MAX_NODES + 1);
    struct aws_linked_list_node *nodes[MAX_NODES];

    for (size_t i = 0; i < num_nodes; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        nodes[i]->next = NULL;
        nodes[i]->prev = NULL;
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* 3. Choose a node that is guaranteed to be in the list */
    struct aws_linked_list_node *node;
    __CPROVER_assume(num_nodes > 0);               /* list must contain at least one node */
    size_t idx = nondet_uint32_t() % num_nodes;
    node = nodes[idx];
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /* Save surrounding links before removal */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* -------------------------------------------------------------------- */
    /* 5. Post‑condition assertions                                            */
    /* -------------------------------------------------------------------- */

    /* 5.1. Changed fields (as per implementation) */
    assert(node->next == NULL);   /* node is reset */
    assert(node->prev == NULL);   /* node is reset */

    /* The surrounding nodes must now point to each other */
    if (old_prev != NULL && old_next != NULL) {
        assert(old_prev->next == old_next);
        assert(old_next->prev == old_prev);
    }

    /* 5.2. Unchanged fields (implicit invariants) */
    /* The list structure itself (head and tail sentinels) must still be the same objects */
    assert(&list.head == &list.head);
    assert(&list.tail == &list.tail);
    /* The memory of nodes other than the removed one must not be altered except for linkage */
    for (size_t i = 0; i < num_nodes; ++i) {
        if (nodes[i] != node && nodes[i] != old_prev && nodes[i] != old_next) {
            /* Their next/prev pointers are unchanged */
            assert(nodes[i]->next == (i + 1 < num_nodes ? nodes[i + 1] : &list.tail));
            assert(nodes[i]->prev == (i == 0 ? &list.head : nodes[i - 1]);
        }
    }

    /* 5.3. Failure path – this function cannot fail (void), so no failure assertions needed */

    /* 5.4. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
    assert(!aws_linked_list_node_is_in_list(node));
}
