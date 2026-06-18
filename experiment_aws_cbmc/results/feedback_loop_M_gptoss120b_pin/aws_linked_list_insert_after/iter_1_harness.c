#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Non‑deterministically decide how many nodes are already in the list */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 3. Allocate those nodes and push them to the back of the list */
    struct aws_linked_list_node *nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < n; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* 4. Choose the 'after' node: either the head sentinel or one of the existing nodes */
    struct aws_linked_list_node *after;
    if (nondet_bool()) {
        after = &list.head;
    } else {
        if (n == 0) {
            after = &list.head;
        } else {
            size_t idx = nondet_size_t();
            __CPROVER_assume(idx < n);
            after = nodes[idx];
        }
    }

    /* 5. Allocate a fresh node to add */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 6. Save old linkage state */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_next_next = NULL;
    if (old_after_next != NULL) {
        old_next_next = old_after_next->next;
    }

    /* 7. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 8. Post‑conditions that must hold after a successful insertion */
    assert(after->next == to_add);                     /* new node follows 'after' */
    assert(to_add->prev == after);                     /* new node points back to 'after' */
    assert(to_add->next == old_after_next);            /* new node links to the former successor */
    assert(old_after_next->prev == to_add);            /* former successor now points back to new node */
    assert(after->prev == old_after_prev);             /* 'after' previous link unchanged */
    if (old_after_next != NULL) {
        assert(old_after_next->next == old_next_next); /* successor's next link unchanged */
    }

    /* 9. The added node must now be considered part of the list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 10. List validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
