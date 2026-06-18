#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a node and add it to the list (ensure at least one node) */
    struct aws_linked_list_node node;
    aws_linked_list_node_reset(&node);
    aws_linked_list_push_back(&list, &node);

    /* 3. Assume the node is in the list */
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    /* 4. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_prev = node.prev;
    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 5. Call the function under test */
    aws_linked_list_remove(&node);

    /* 6. Post‑condition: the removed node's pointers are reset */
    assert(node.prev == NULL);
    assert(node.next == NULL);

    /* 7. Post‑condition: neighboring nodes now point to each other */
    if (old_prev != NULL) {
        assert(old_prev->next == old_next);
    }
    if (old_next != NULL) {
        assert(old_next->prev == old_prev);
    }

    /* 8. Unchanged sentinel fields */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* 9. If the removed node was the first element, head.next updates */
    if (old_prev == &list.head) {
        assert(list.head.next == old_next);
    } else {
        assert(list.head.next == old_head_next);
    }

    /* 10. If the removed node was the last element, tail.prev updates */
    if (old_next == &list.tail) {
        assert(list.tail.prev == old_prev);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    /* 11. List validity invariant must hold after removal */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
