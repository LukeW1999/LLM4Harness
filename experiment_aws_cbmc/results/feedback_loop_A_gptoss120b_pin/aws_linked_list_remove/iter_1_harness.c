#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Insert a node into the list so that we have a removable element */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    /* The node is freshly allocated; its pointers are irrelevant before insertion */
    aws_linked_list_push_back(&list, node);

    /* 3. Save old state before removal */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call function under test */
    aws_linked_list_remove(node);

    /* 5. Post‑condition: the removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(!aws_linked_list_node_is_in_list(node));

    /* 6. Post‑condition: neighboring nodes now bypass the removed node */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 7. Unchanged fields of the list (sentinel connections) */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 8. The list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));
}
