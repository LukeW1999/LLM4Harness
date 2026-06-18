#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Ensure the list is non‑empty and pick a real node */
    __CPROVER_assume(list.head.next != &list.tail);
    struct aws_linked_list_node *node = list.head.next;

    /* 3. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node = *node;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Postconditions */
    assert(result == node->next);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 6. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
