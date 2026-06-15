#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 4
#endif

void aws_linked_list_pop_back_harness(void) {
    /* Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* If the list is non‑empty, remember the node that will be removed */
    struct aws_linked_list_node *old_last = NULL;
    struct aws_linked_list_node *old_last_prev = NULL;
    if (old_tail_prev != &list.head) {
        old_last = old_tail_prev;
        old_last_prev = old_last->prev;
    }

    /* Call the function under test */
    struct aws_linked_list_node *removed = aws_linked_list_pop_back(&list);

    /* Post‑condition: list must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* The head node's prev is always NULL and the tail node's next is always NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    if (old_head_next == &list.tail) {
        /* The list was originally empty */
        assert(removed == NULL);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* The list was originally non‑empty */
        assert(removed == old_last);
        /* The removed node's links are cleared */
        assert(removed->prev == NULL);
        assert(removed->next == NULL);
        /* New tail points to the previous node */
        assert(list.tail.prev == old_last_prev);
        /* The previous node now points to the tail */
        assert(old_last_prev->next == &list.tail);
        /* Head of the list is unchanged */
        assert(list.head.next == old_head_next);
    }
}
