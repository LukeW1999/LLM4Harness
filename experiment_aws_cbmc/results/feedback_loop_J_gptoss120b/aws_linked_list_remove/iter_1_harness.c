#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a node and insert it so the list is non‑empty */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_push_back(&list, node);

    /* 3. Assume the list is still valid after insertion */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 4. Save old state needed for post‑condition checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 5. Call the function under test */
    aws_linked_list_remove(node);

    /* 6. Post‑conditions */

    /* 6a. The removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(aws_linked_list_node_is_in_list(node) == false);

    /* 6b. Adjacent nodes are linked together */
    if (old_prev != NULL) {
        assert(old_prev->next == old_next);
    }
    if (old_next != NULL) {
        assert(old_next->prev == old_prev);
    }

    /* 6c. Sentinel links that are not part of the removed node remain unchanged */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 6d. The list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
