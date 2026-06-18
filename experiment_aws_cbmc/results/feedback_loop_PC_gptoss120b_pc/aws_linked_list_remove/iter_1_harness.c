#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a nondet number of nodes (bounded) */
    size_t num_nodes = nondet_size_t();
    __CPROVER_assume(num_nodes > 0);
    __CPROVER_assume(num_nodes <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *nodes = malloc(num_nodes * sizeof(struct aws_linked_list_node));
    __CPROVER_assume(nodes != NULL);

    /* 3. Initialize each node (reset) */
    for (size_t i = 0; i < num_nodes; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
    }

    /* 4. Build a valid list by pushing each node to the back */
    for (size_t i = 0; i < num_nodes; ++i) {
        aws_linked_list_push_back(&list, &nodes[i]);
    }
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Snapshot old state */
    struct aws_linked_list old_list = list;

    struct aws_linked_list_node **old_next = malloc(num_nodes * sizeof(struct aws_linked_list_node *));
    struct aws_linked_list_node **old_prev = malloc(num_nodes * sizeof(struct aws_linked_list_node *));
    __CPROVER_assume(old_next != NULL && old_prev != NULL);
    for (size_t i = 0; i < num_nodes; ++i) {
        old_next[i] = nodes[i].next;
        old_prev[i] = nodes[i].prev;
    }

    /* 6. Choose a node to remove (must be a real node, not sentinel) */
    size_t idx = nondet_uint32_t() % num_nodes;
    struct aws_linked_list_node *node = &nodes[idx];
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    struct aws_linked_list_node *prev = node->prev;
    struct aws_linked_list_node *next = node->next;

    /* 7. Call function under test */
    aws_linked_list_remove(node);

    /* 8. Post‑condition: removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 9. Post‑condition: surrounding nodes are linked together */
    assert(prev->next == next);
    assert(next->prev == prev);

    /* 10. The removed node is no longer in the list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* 11. All other nodes retain their original connections */
    for (size_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *cur = &nodes[i];
        if (cur != node && cur != prev && cur != next) {
            assert(cur->next == old_next[i]);
            assert(cur->prev == old_prev[i]);
        }
    }

    /* 12. List validity invariant holds after removal */
    assert(aws_linked_list_is_valid(&list));

    /* 13. List sentinel nodes (head/tail) unchanged except for possible linkage updates already verified */
    assert(list.head.prev == old_list.head.prev);
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(list.tail.next == old_list.tail.next);
}
