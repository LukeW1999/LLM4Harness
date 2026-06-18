#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_next_next = NULL;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    if (old_head_next != &list.tail) {
        old_head_next_next = old_head_next->next;
    }

    /* Call function under test */
    aws_linked_list_pop_front(&list);

    /* List must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* Invariant: head.prev is always NULL and tail.next is always NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    if (old_head_next == &list.tail) {
        /* List was empty: must be unchanged */
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* List was non‑empty: first node removed */
        assert(list.head.next == old_head_next_next);
        if (old_head_next_next != &list.tail) {
            /* New first node must point back to head */
            assert(old_head_next_next->prev == &list.head);
        } else {
            /* List became empty, tail.prev must point to head */
            assert(list.tail.prev == &list.head);
        }

        /* Tail.prev unchanged unless the removed node was the only element */
        if (old_head_next == old_tail_prev) {
            assert(list.tail.prev == &list.head);
        } else {
            assert(list.tail.prev == old_tail_prev);
        }
    }
}
