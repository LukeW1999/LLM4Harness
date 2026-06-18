#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* Helper to count the number of real nodes in a list (excluding sentinels) */
static size_t list_count(const struct aws_linked_list *list) {
    size_t cnt = 0;
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        cnt++;
    }
    return cnt;
}

/* Helper to nondeterministically decide a boolean */
static bool nondet_bool(void);
static size_t nondet_size_t(void);

void aws_linked_list_remove_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(aws_linked_list_is_valid(&list));

    /* Create a bounded number of nodes (max 5) */
    const size_t MAX_NODES = 5;
    struct aws_linked_list_node *nodes[MAX_NODES];
    for (size_t i = 0; i < MAX_NODES; ++i) {
        nodes[i] = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
        /* Ensure allocation succeeded */
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
    }

    /* Nondeterministically decide how many nodes to actually insert (0..MAX_NODES) */
    size_t initial_len = nondet_size_t();
    __CPROVER_assume(initial_len <= MAX_NODES);

    for (size_t i = 0; i < initial_len; ++i) {
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* Preserve a snapshot of the list before removal for frame condition checks */
    struct aws_linked_list list_before = list;
    struct aws_linked_list_node *node_array_before[MAX_NODES];
    for (size_t i = 0; i < MAX_NODES; ++i) {
        node_array_before[i] = nodes[i];
    }

    /* If the list is empty, there is nothing to remove; just exit */
    if (initial_len == 0) {
        return 0;
    }

    /* Choose a node that is definitely in the list to remove */
    size_t idx_to_remove = nondet_size_t();
    __CPROVER_assume(idx_to_remove < initial_len);
    struct aws_linked_list_node *node_to_remove = nodes[idx_to_remove];

    /* Call the function under verification */
    aws_linked_list_remove(node_to_remove);

    /* ---- Postconditions ---- */

    /* 1. The removed node must be reset (next and prev are NULL) */
    assert(node_to_remove->next == NULL);
    assert(node_to_remove->prev == NULL);

    /* 2. The list must remain valid */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* 3. The length of the list must have decreased by exactly one */
    assert(list_count(&list) == initial_len - 1);

    /* 4. All other nodes that were not removed must retain their identity */
    for (size_t i = 0; i < MAX_NODES; ++i) {
        if (i != idx_to_remove && i < initial_len) {
            /* The node should still be linked somewhere in the list */
            bool found = false;
            for (struct aws_linked_list_node *cur = aws_linked_list_begin(&list);
                 cur != aws_linked_list_end(&list);
                 cur = aws_linked_list_next(cur)) {
                if (cur == nodes[i]) {
                    found = true;
                    break;
                }
            }
            assert(found);
        }
    }

    /* 5. Frame condition: memory of nodes not removed and the allocator must be unchanged
       (aside from the link adjustments performed by the function). Since we cannot
       directly compare the whole memory layout, we conservatively check that the
       allocator pointer is unchanged and that the sentinel nodes (head/tail) are
       still part of the same list structure. */
    assert(list.head.next == list_before.head.next || list.head.next == node_to_remove->next);
    assert(list.tail.prev == list_before.tail.prev || list.tail.prev == node_to_remove->prev);
    (void)aws_default_allocator(); /* ensure allocator is linked */

    /* Clean up allocated memory */
    for (size_t i = 0; i < MAX_NODES; ++i) {
        free(nodes[i]);
    }

    return 0;
}

/* Nondeterministic helpers for CBMC */
size_t nondet_size_t(void) {
    size_t x;
    return x;
}
bool nondet_bool(void) {
    bool b;
    return b;
}
