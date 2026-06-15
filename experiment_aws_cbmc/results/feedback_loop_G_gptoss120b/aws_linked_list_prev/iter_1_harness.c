#include <assert.h>
#include <stddef.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node to pass to the function.
       It may be the head sentinel, the tail sentinel, or the first real node. */
    struct aws_linked_list_node *node;
    uint8_t choice = nondet_uint8_t() % 3;
    if (choice == 0) {
        node = &list.head;
    } else if (choice == 1) {
        node = &list.tail;
    } else {
        node = list.head.next;               /* could be &list.tail if list is empty */
    }

    /* 3. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_node_prev = node->prev;
    struct aws_linked_list_node *old_node_next = node->next;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Post‑condition: return value equals the node's prev pointer */
    assert(result == node->prev);

    /* 6. Unchanged fields of the list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 7. Unchanged fields of the node itself */
    assert(node->prev == old_node_prev);
    assert(node->next == old_node_next);

    /* 8. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
