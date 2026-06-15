/* Contract:
Preconditions:
- after != NULL
- to_add != NULL
- after->next != NULL
- The list containing 'after' is a valid doubly‑linked list (aws_linked_list_is_valid and aws_linked_list_is_valid_deep hold).

Postconditions (validity):
- The list remains a valid doubly‑linked list after insertion.
- to_add is correctly linked: to_add->prev == after,
  to_add->next == old_next,
  after->next == to_add,
  old_next->prev == to_add.

Postconditions (length):
- The number of user‑nodes in the list is increased by exactly one.

Postconditions (frame):
- All nodes other than 'after', 'to_add' and the original after->next retain their original next/prev pointers. */

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_INITIAL_NODES 5

void aws_linked_list_insert_after_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Build an initial list with a nondeterministic length */
    size_t init_len = __CPROVER_nondet_uint();
    if (init_len > MAX_INITIAL_NODES) {
        init_len = MAX_INITIAL_NODES;
    }

    struct aws_linked_list_node *nodes[MAX_INITIAL_NODES];
    struct aws_linked_list_node *prev_before[MAX_INITIAL_NODES];
    struct aws_linked_list_node *next_before[MAX_INITIAL_NODES];

    for (size_t i = 0; i < init_len; ++i) {
        nodes[i] = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* Choose a node 'after' that is guaranteed to be in the list.
       If the list is empty we use the head sentinel. */
    struct aws_linked_list_node *after;
    if (init_len == 0) {
        after = &list.head;
    } else {
        size_t idx = __CPROVER_nondet_uint() % init_len;
        after = nodes[idx];
    }

    /* Allocate the node to be inserted */
    struct aws_linked_list_node *to_add = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);

    /* Record the original next pointer of 'after' */
    struct aws_linked_list_node *old_next = after->next;

    /* Record the length of the list before insertion */
    size_t len_before = 0;
    for (struct aws_linked_list_node *it = aws_linked_list_begin(&list);
         it != aws_linked_list_end(&list);
         it = aws_linked_list_next(it)) {
        ++len_before;
    }

    /* Record the next/prev of all existing nodes for frame condition */
    for (size_t i = 0; i < init_len; ++i) {
        prev_before[i] = nodes[i]->prev;
        next_before[i] = nodes[i]->next;
    }

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Verify post‑conditions */

    /* Validity of the list */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Correct linking of the inserted node */
    assert(to_add->prev == after);
    assert(to_add->next == old_next);
    assert(after->next == to_add);
    assert(old_next->prev == to_add);

    /* Length increased by exactly one */
    size_t len_after = 0;
    for (struct aws_linked_list_node *it = aws_linked_list_begin(&list);
         it != aws_linked_list_end(&list);
         it = aws_linked_list_next(it)) {
        ++len_after;
    }
    assert(len_after == len_before + 1);

    /* Frame condition: all other nodes unchanged */
    for (size_t i = 0; i < init_len; ++i) {
        struct aws_linked_list_node *node = nodes[i];
        if (node != after && node != old_next) {
            assert(node->prev == prev_before[i]);
            assert(node->next == next_before[i]);
        }
    }

    return 0;
}
