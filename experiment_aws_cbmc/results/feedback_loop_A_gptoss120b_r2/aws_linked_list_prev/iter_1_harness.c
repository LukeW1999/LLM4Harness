#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);

    /* 2. Build a non‑empty list (at least one node) */
    uint32_t num_nodes = nondet_uint32_t();
    __CPROVER_assume(num_nodes > 0);
    __CPROVER_assume(num_nodes <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node **nodes = malloc(num_nodes * sizeof(*nodes));
    __CPROVER_assume(nodes != NULL);

    for (uint32_t i = 0; i < num_nodes; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        /* start with a clean node */
        nodes[i]->next = NULL;
        nodes[i]->prev = NULL;
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* 3. Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 4. Save old state of the list (head/tail nodes) */
    struct aws_linked_list old = list;

    /* 5. Choose a node that is part of the list (first element) */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != &list.tail); /* ensure we have a real element */

    /* 6. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 7. Post‑condition: result must be the previous pointer of the input node */
    assert(result == node->prev);

    /* 8. Unchanged fields of the list structure */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 9. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* 10. Clean up */
    for (uint32_t i = 0; i < num_nodes; ++i) {
        free(nodes[i]);
    }
    free(nodes);
}
