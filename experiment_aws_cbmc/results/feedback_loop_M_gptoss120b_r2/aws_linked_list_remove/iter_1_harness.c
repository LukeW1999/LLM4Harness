#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node and insert it into the list */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_push_back(&list, node);

    /* 3. Save old state before removal */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list old_list = list; /* copies sentinel nodes */

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Post‑condition: the removed node is reset */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* 6. Post‑condition: surrounding nodes are linked together */
    if (old_prev != NULL) {
        assert(old_prev->next == old_next);
    }
    if (old_next != NULL) {
        assert(old_next->prev == old_prev);
    }

    /* 7. The node is no longer considered part of a list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* 8. Unchanged sentinel fields */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 9. Head and tail connections adjust only if the removed node was at an end */
    if (old_prev == &list.head) {
        assert(list.head.next == old_next);
    } else {
        assert(list.head.next == old_list.head.next);
    }

    if (old_next == &list.tail) {
        assert(list.tail.prev == old_prev);
    } else {
        assert(list.tail.prev == old_list.tail.prev);
    }

    /* 10. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
