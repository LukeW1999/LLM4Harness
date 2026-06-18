#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare and initialize the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state before calling */
    /* Save the old last node (tail.prev before push) */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* Save head state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* Changed fields: node is now the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* New node's next points to tail */
    assert(node->next == &list.tail);

    /* New node's prev points to old last node */
    assert(node->prev == old_last);

    /* Old last node's next now points to new node */
    assert(old_last->next == node);

    /* Unchanged fields: head sentinel fields that should not change */
    assert(list.head.prev == old_head_prev);  /* head.prev stays NULL */
    assert(list.tail.next == old_tail_next);  /* tail.next stays NULL */

    /* head.next only changes if list was empty before push */
    /* If list was empty: old_last == &list.head, so head.next should now be node */
    /* If list was non-empty: head.next should remain unchanged */
    if (old_last == &list.head) {
        /* List was empty before push */
        assert(list.head.next == node);
    } else {
        /* List was non-empty before push */
        assert(list.head.next == old_head_next);
    }

    /* 6. Assert validity invariants */
    assert(aws_linked_list_is_valid(&list));

    /* Node linkage validity */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
}
