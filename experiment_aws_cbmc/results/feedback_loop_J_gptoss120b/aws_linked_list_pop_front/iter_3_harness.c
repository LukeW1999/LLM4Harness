#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Harness for aws_linked_list_pop_front */
void aws_linked_list_pop_front_harness(void) {
    struct aws_linked_list list;

    /* Allocate a linked list with a nondeterministic number of items */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save pointers to the original front node and its successor */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = old_front->next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* The list must remain valid */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next->prev == &list.head);
    assert(list.tail.prev->next == &list.tail);

    if (old_front == &list.tail) {
        /* The original list was empty */
        assert(popped == NULL);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* The original list was non‑empty */
        assert(popped != NULL);
        assert(popped->prev == NULL);
        assert(popped->next == NULL);

        /* The new front of the list is the former second node */
        assert(list.head.next == old_second);

        if (old_second == &list.tail) {
            /* The original list had exactly one element */
            assert(list.head.next == &list.tail);
            assert(list.tail.prev == &list.head);
        } else {
            /* The original list had more than one element */
            assert(list.head.next->prev == &list.head);
            assert(list.tail.prev == old_tail_prev);
        }
    }
}
