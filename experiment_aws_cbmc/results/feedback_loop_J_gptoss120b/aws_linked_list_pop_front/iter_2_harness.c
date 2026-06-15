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

    /* Save the original state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_front = list.head.next;

    /* Call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* Post‑condition: the list must remain valid */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next->prev == &list.head);
    assert(list.tail.prev->next == &list.tail);

    if (old_front == &old.tail) {
        /* The original list was empty */
        assert(popped == NULL);
        assert(list.head.next == old.head.next);
        assert(list.tail.prev == old.tail.prev);
    } else {
        /* The original list was non‑empty */
        assert(popped != NULL);
        assert(popped->prev == NULL);
        assert(popped->next == NULL);

        /* The new front of the list is the former second node */
        assert(list.head.next == old_front->next);

        if (old_front->next == &old.tail) {
            /* The original list had exactly one element */
            assert(list.head.next == &list.tail);
            assert(list.tail.prev == &list.head);
        } else {
            /* The original list had more than one element */
            assert(list.head.next->prev == &list.head);
            assert(list.tail.prev == old.tail.prev);
        }
    }
}
