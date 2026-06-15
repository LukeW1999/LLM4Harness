/*=== Contract for aws_linked_list_pop_front ===
Preconditions:
  - The list pointer passed to aws_linked_list_pop_front is non‑NULL.
  - The list is a valid doubly‑linked list (aws_linked_list_is_valid returns true).
  - The list is non‑empty (aws_linked_list_empty returns false).

Postconditions (validity):
  - The returned node pointer is non‑NULL.
  - The returned node’s next and prev fields are NULL (node has been reset).
  - The list remains a valid doubly‑linked list (aws_linked_list_is_valid returns true).

Postconditions (length):
  - Let L_before be the number of user nodes in the list before the call.
    Let L_after be the number after the call.
    Then L_after == L_before - 1.

Postconditions (frame):
  - Apart from the popped node (which is reset) and the list’s head/tail
    sentinel pointers, no other memory locations are modified.
  - All remaining nodes retain their original next/prev relationships,
    except that the new first node’s prev now points to the list head.===*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* nondet helpers */
bool nondet_bool(void);
size_t nondet_size_t(void);

/* maximum number of extra nodes we will create in the harness */
#define MAX_EXTRA_NODES 3

void aws_linked_list_pop_front_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate at least one node that will be popped */
    struct aws_linked_list_node *node0 = malloc(sizeof(*node0));
    __CPROVER_assume(node0 != NULL);
    aws_linked_list_node_reset(node0);
    aws_linked_list_push_front(&list, node0);

    /* Allocate a nondeterministic number (0..MAX_EXTRA_NODES) of additional nodes */
    struct aws_linked_list_node *extra_nodes[MAX_EXTRA_NODES];
    size_t extra_count = 0;
    for (size_t i = 0; i < MAX_EXTRA_NODES; ++i) {
        if (nondet_bool()) {
            struct aws_linked_list_node *n = malloc(sizeof(*n));
            __CPROVER_assume(n != NULL);
            aws_linked_list_node_reset(n);
            /* push to the back so order of remaining nodes is deterministic */
            aws_linked_list_push_back(&list, n);
            extra_nodes[extra_count++] = n;
        }
    }

    /* Record the list length before the pop */
    size_t len_before = 0;
    for (struct aws_linked_list_node *it = aws_linked_list_begin(&list);
         it != aws_linked_list_end(&list);
         it = aws_linked_list_next(it)) {
        ++len_before;
    }
    __CPROVER_assert(len_before >= 1, "precondition: list non‑empty");

    /* Record the next/prev of each extra node before the operation */
    struct aws_linked_list_node *saved_next[MAX_EXTRA_NODES];
    struct aws_linked_list_node *saved_prev[MAX_EXTRA_NODES];
    for (size_t i = 0; i < extra_count; ++i) {
        saved_next[i] = extra_nodes[i]->next;
        saved_prev[i] = extra_nodes[i]->prev;
    }

    /* Preconditions for the function under test */
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* Postcondition: returned node is non‑NULL and reset */
    assert(popped != NULL);
    assert(popped->next == NULL);
    assert(popped->prev == NULL);

    /* Postcondition: list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition: length decreased by one */
    size_t len_after = 0;
    for (struct aws_linked_list_node *it = aws_linked_list_begin(&list);
         it != aws_linked_list_end(&list);
         it = aws_linked_list_next(it)) {
        ++len_after;
    }
    assert(len_after + 1 == len_before);

    /* Determine the new first node, if any */
    struct aws_linked_list_node *new_first = NULL;
    if (len_after > 0) {
        new_first = aws_linked_list_begin(&list);
    }

    /* Frame condition: extra nodes unchanged, except possible prev of new first */
    for (size_t i = 0; i < extra_count; ++i) {
        assert(extra_nodes[i]->next == saved_next[i]);
        if (extra_nodes[i] == new_first) {
            /* The new first node’s prev should now point to the list head */
            assert(extra_nodes[i]->prev == &list.head);
        } else {
            assert(extra_nodes[i]->prev == saved_prev[i]);
        }
    }

    /* Frame condition: head and tail sentinels are correctly updated */
    if (len_after == 0) {
        /* List became empty */
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* New first element should be the node that was second before */
        struct aws_linked_list_node *expected_first = aws_linked_list_begin(&list);
        if (extra_count > 0) {
            assert(expected_first == extra_nodes[0]);
        } else {
            /* only node0 existed, list would be empty – already handled */
        }
    }

    return 0;
}
