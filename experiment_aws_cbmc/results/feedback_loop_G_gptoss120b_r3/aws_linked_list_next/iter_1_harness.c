#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a non‑NULL node that could be part of the list */
    struct aws_linked_list_node *node = nondet_uint8_t() ? &list.head : &list.tail;
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list old_list = list;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Post‑conditions */

    /* The function simply returns node->next */
    assert(result == old_node.next);

    /* No fields of the node are modified */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* The list structure itself is unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
}
