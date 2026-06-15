#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 5

static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        ++len;
        cur = cur->next;
    }
    return len;
}

void aws_linked_list_pop_front_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assert(aws_linked_list_is_valid(&list), "list valid after init");

    size_t n;
    __CPROVER_assume(n > 0 && n < MAX_NODES);

    struct aws_linked_list_node nodes[MAX_NODES];
    struct {
        struct aws_linked_list_node *next;
        struct aws_linked_list_node *prev;
    } snapshots[MAX_NODES];

    for (size_t i = 0; i < n; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_front(&list, &nodes[i]);
    }

    size_t pre_len = list_length(&list);
    struct aws_linked_list_node *pre_head = list.head.next;
    struct aws_linked_list_node *pre_tail = list.tail.prev;
    struct aws_linked_list_node *second = NULL;
    if (pre_len > 1) {
        second = pre_head->next; /* capture node that will become new head */
    }

    for (size_t i = 0; i < n; ++i) {
        snapshots[i].next = nodes[i].next;
        snapshots[i].prev = nodes[i].prev;
    }

    struct aws_linked_list_node *ret = aws_linked_list_pop_front(&list);

    __CPROVER_assert(ret != NULL, "pop_front returns non‑NULL");
    __CPROVER_assert(ret->next == NULL && ret->prev == NULL, "popped node cleared");

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list valid after pop");
    size_t post_len = list_length(&list);
    __CPROVER_assert(post_len + 1 == pre_len, "length decreased by one");

    if (pre_len > 1) {
        __CPROVER_assert(list.head.next == second, "new head is former second");
        __CPROVER_assert(list.tail.prev == pre_tail, "tail unchanged");
    } else {
        __CPROVER_assert(list.head.next == &list.tail, "list empty after single pop");
        __CPROVER_assert(list.tail.prev == &list.head, "tail points to head after empty");
    }

    for (size_t i = 0; i < n; ++i) {
        if (&nodes[i] != ret && &nodes[i] != second) {
            __CPROVER_assert(nodes[i].next == snapshots[i].next, "node next unchanged");
            __CPROVER_assert(nodes[i].prev == snapshots[i].prev, "node prev unchanged");
        }
    }
}
