#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node to query */
    struct aws_linked_list_node *node;
    /* nondet choice among head, tail, or first real element (if any) */
    node = nondet_bool() ? &list.head
                         : (nondet_bool() ? &list.tail : list.head.next);
    __CPROVER_assume(node != NULL);               /* node must be readable */

    /* 3. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Post‑condition: return value equals the node's prev pointer */
    assert(result == old_node.prev);

    /* 6. Unchanged fields: list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 7. Unchanged fields: the queried node */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 8. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
