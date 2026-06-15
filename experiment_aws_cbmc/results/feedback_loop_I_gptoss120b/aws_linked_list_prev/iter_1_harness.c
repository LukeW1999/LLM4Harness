#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node and embed it into the list (single‑element list) */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* set up sentinel links */
    list.head.next = node;
    list.head.prev = NULL;
    list.tail.prev = node;
    list.tail.next = NULL;

    /* link the node */
    node->prev = &list.head;
    node->next = &list.tail;

    /* 3. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Post‑conditions */

    /* result must be the previous pointer of the input node */
    assert(result == old_node.prev);

    /* The list structure must remain unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* The node itself must remain unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 6. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
