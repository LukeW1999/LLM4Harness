/*=== Contract for aws_linked_list_push_front ===
Preconditions:
  - list is a valid, initialized aws_linked_list (aws_linked_list_is_valid(list) == true)
  - node is a non‑NULL pointer to an aws_linked_list_node whose next and prev are NULL
  - node is not currently part of any list (aws_linked_list_node_is_in_list(node) == false)

Postconditions (validity):
  - aws_linked_list_is_valid(list) == true after the call
  - list.head.next points to node
  - node->prev == &list.head
  - node->next points to the element that was previously first (or &list.tail if the list was empty)

Postconditions (length):
  - let len_before be the number of user nodes in list before the call
  - let len_after be the number of user nodes after the call
  - len_after == len_before + 1

Postconditions (frame):
  - All nodes that were not the inserted node and not the former first node retain their next and prev pointers
  - The tail.prev pointer is unchanged unless the list was empty, in which case it becomes node
===*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

/* Helper to compute list length (number of user nodes) */
static size_t list_length(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

/* Helper to nondeterministically build a valid list with up to MAX_NODES elements */
#define MAX_NODES 5
static void build_nondet_list(struct aws_linked_list *list) {
    aws_linked_list_init(list);
    size_t n = __CPROVER_nondet_uint();
    __CPROVER_assume(n <= MAX_NODES);
    struct aws_linked_list_node *prev = &list->head;
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        /* link node */
        prev->next = node;
        node->prev = prev;
        prev = node;
    }
    /* close the list */
    prev->next = &list->tail;
    list->tail.prev = prev;
}

/* CBMC harness */
void aws_linked_list_push_front_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node *node;

    /* Build a nondet valid list */
    build_nondet_list(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Capture pre‑state */
    size_t len_before = list_length(&list);
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Allocate a fresh node not in any list */
    node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(node));

    /* Call the function under test */
    aws_linked_list_push_front(&list, node);

    /* ---- Postcondition checks ---- */

    /* Validity */
    assert(aws_linked_list_is_valid(&list));

    /* Length */
    assert(list_length(&list) == len_before + 1);

    /* Front pointer */
    assert(list.head.next == node);
    assert(node->prev == &list.head);

    if (len_before == 0) {
        /* List was empty, node should point to tail */
        assert(node->next == &list.tail);
        assert(list.tail.prev == node);
    } else {
        /* List was non‑empty, node should point to former first */
        assert(node->next == old_first);
        assert(old_first->prev == node);
        /* Tail unchanged */
        assert(list.tail.prev == old_tail_prev);
    }

    /* Frame: nodes other than node and (if any) old_first retain their links */
    struct aws_linked_list_node *cur = list.head.next;
    while (cur != &list.tail) {
        if (cur != node && cur != old_first) {
            /* For any untouched node, its next and prev must still be consistent */
            assert(aws_linked_list_node_next_is_valid(cur));
            assert(aws_linked_list_node_prev_is_valid(cur));
        }
        cur = cur->next;
    }

    return 0;
}
