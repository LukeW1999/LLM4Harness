#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Non‑deterministically decide how many nodes to put in the list */
    size_t max_nodes = MAX_LINKED_LIST_ITEM_ALLOCATION;
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= max_nodes);

    struct aws_linked_list_node nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    struct aws_linked_list_node *node_ptrs[MAX_LINKED_LIST_ITEM_ALLOCATION];

    for (size_t i = 0; i < n; ++i) {
        node_ptrs[i] = &nodes[i];
        nodes[i].next = NULL;
        nodes[i].prev = NULL;
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    /* 3. Save old state of the whole list */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_nexts[MAX_LINKED_LIST_ITEM_ALLOCATION];
    struct aws_linked_list_node *old_prevs[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < n; ++i) {
        old_nexts[i] = nodes[i].next;
        old_prevs[i] = nodes[i].prev;
    }

    /* 4. Choose a node (head, tail, or any element) */
    size_t total_choices = n + 2;               /* head, tail, plus n elements */
    size_t choice = nondet_size_t();
    __CPROVER_assume(choice < total_choices);

    struct aws_linked_list_node *node;
    if (choice == 0) {
        node = &list.head;
    } else if (choice == 1) {
        node = &list.tail;
    } else {
        node = node_ptrs[choice - 2];
    }

    /* 5. Save the node's next pointer before the call */
    struct aws_linked_list_node *old_node_next = node->next;

    /* 6. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 7. Post‑conditions */
    /* 7a. The returned pointer must be the original next pointer */
    assert(result == old_node_next);

    /* 7b. No fields of the list or its nodes may have changed */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    for (size_t i = 0; i < n; ++i) {
        assert(nodes[i].next == old_nexts[i]);
        assert(nodes[i].prev == old_prevs[i]);
    }

    /* 8. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
