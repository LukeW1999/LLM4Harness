#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/* Helper to count nodes in a list (excluding sentinels) */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

/* Helper to check that a given node is not reachable from the list */
static bool node_not_in_list(const struct aws_linked_list *list,
                             const struct aws_linked_list_node *node) {
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        if (cur == node) {
            return false;
        }
        cur = cur->next;
    }
    return true;
}

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Choose a non‑sentinel node that is part of the list */
    struct aws_linked_list_node *node = NULL;

    if (!aws_linked_list_empty(&list)) {
        size_t idx = nondet_size_t();
        __CPROVER_assume(idx < MAX_LINKED_LIST_ITEM_ALLOCATION);

        struct aws_linked_list_node *cur = list.head.next;
        size_t i = 0;
        while (i < idx && cur != &list.tail) {
            cur = cur->next;
            i++;
        }
        if (cur != &list.tail) {
            node = cur;
        }
    }

    /* Ensure we have a removable node (not a sentinel) */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head);
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(node->prev->next == node);
    __CPROVER_assume(node->next->prev == node);

    /* 3. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node old_node = *node;
    size_t old_count = count_nodes(&list);

    /* 4. Call function under test */
    aws_linked_list_remove(node);

    /* 5. Post‑condition: removed node is reset */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* 6. Post‑condition: surrounding links are updated */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 7. Post‑condition: list length decreased by one */
    assert(count_nodes(&list) + 1 == old_count);

    /* 8. Post‑condition: removed node is no longer reachable */
    assert(node_not_in_list(&list, node));

    /* 9. Unchanged fields of unrelated nodes (deep check) */
    assert(aws_linked_list_is_valid_deep(&list));

    /* 10. Unchanged sentinel connections (except where they may have been updated) */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
}
