#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node that will be removed */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);

    /* 3. Insert the node into the list (so it is a real list element) */
    aws_linked_list_push_back(&list, node);

    /* 4. Optionally add another node to make the list longer */
    struct aws_linked_list_node *other = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(other != NULL);
    aws_linked_list_node_reset(other);
    if (nondet_bool()) {
        aws_linked_list_push_back(&list, other);
    } else {
        aws_linked_list_push_front(&list, other);
    }

    /* 5. Ensure the list is still valid after construction */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 6. Save old state before removal */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_head_next = old.head.next;
    struct aws_linked_list_node *old_tail_prev = old.tail.prev;

    /* 7. Call the function under test */
    aws_linked_list_remove(node);

    /* 8. Post‑condition: the removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 9. Post‑condition: adjacency links are updated */
    if (old_prev != NULL && old_next != NULL) {
        assert(old_prev->next == old_next);
        assert(old_next->prev == old_prev);
    }

    /* 10. Post‑condition: list sentinel links that never change */
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* 11. Post‑condition: head.next changes only if the removed node was first */
    if (old_prev == &list.head) {
        assert(list.head.next == old_next);
    } else {
        assert(list.head.next == old_head_next);
    }

    /* 12. Post‑condition: tail.prev changes only if the removed node was last */
    if (old_next == &list.tail) {
        assert(list.tail.prev == old_prev);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    /* 13. Validity invariants must hold after removal */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
