#include <proof_helpers/make_common_data_structures.h>

#define MAX_NODES 5

static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        cnt++;
    }
    return cnt;
}

static bool node_in_list(const struct aws_linked_list *list,
                         const struct aws_linked_list_node *target) {
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        if (cur == target) {
            return true;
        }
    }
    return false;
}

void aws_linked_list_remove_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Create a nondeterministic number of nodes (bounded) */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_NODES);
    __CPROVER_assume(n > 0); /* ensure list is non‑empty for removal */

    struct aws_linked_list_node nodes[MAX_NODES];
    struct aws_linked_list_node nodes_before[MAX_NODES];

    /* Populate the list */
    for (size_t i = 0; i < n; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    /* Choose a node that is guaranteed to be in the list */
    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < n);
    struct aws_linked_list_node *node = &nodes[idx];
    __CPROVER_assume(node_in_list(&list, node));

    /* Snapshot of pre‑state for frame condition checks */
    struct aws_linked_list list_before = list;
    for (size_t i = 0; i < n; ++i) {
        nodes_before[i].next = nodes[i].next;
        nodes_before[i].prev = nodes[i].prev;
    }

    /* Invoke the function under test */
    aws_linked_list_remove(node);

    /* Post‑condition 1: the removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* Post‑condition 2: the list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Post‑condition 3: length invariant (list size decreased by one) */
    size_t len_before = count_nodes(&list_before);
    size_t len_after = count_nodes(&list);
    assert(len_before == len_after + 1);

    /* Post‑condition 4: all other nodes remain in the list */
    for (size_t i = 0; i < n; ++i) {
        if (&nodes[i] != node) {
            assert(node_in_list(&list, &nodes[i]));
        }
    }

    /* Frame condition: memory of nodes other than the removed one is unchanged
       (except for their next/prev pointers which may have been updated by the removal) */
    for (size_t i = 0; i < n; ++i) {
        if (&nodes[i] != node) {
            if (nodes_before[i].next == node) {
                assert(nodes[i].next == node->next);
            } else {
                assert(nodes[i].next == nodes_before[i].next);
            }
            if (nodes_before[i].prev == node) {
                assert(nodes[i].prev == node->prev);
            } else {
                assert(nodes[i].prev == nodes_before[i].prev);
            }
        }
    }
}
