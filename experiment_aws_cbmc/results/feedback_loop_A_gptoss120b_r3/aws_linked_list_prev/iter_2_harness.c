#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state of the list */
    struct aws_linked_list old = list;

    /* 3. Choose a node that belongs to the list and satisfies the pre‑condition */
    struct aws_linked_list_node *node = &list.tail;   /* tail always has a valid prev */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node->prev != NULL);            /* satisfies aws_linked_list_node_prev_is_valid */

    /* Save old state of the chosen node */
    struct aws_linked_list_node old_node = *node;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Post‑conditions */

    /* The function must return the previous pointer of the input node */
    assert(result == node->prev);

    /* The list must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The node itself must remain unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 6. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
