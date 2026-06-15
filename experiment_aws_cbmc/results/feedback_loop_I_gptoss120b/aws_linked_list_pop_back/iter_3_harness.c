#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

/* Harness for aws_linked_list_pop_back */
void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;

    /* Allocate and bound the list */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last  = list.tail.prev;
    struct aws_linked_list_node *old_last_prev = NULL;
    if (old_last != &list.head) {
        old_last_prev = old_last->prev;
    }
    bool was_empty = aws_linked_list_empty(&list);

    /* Call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    /* Post‑condition checks */
    assert(aws_linked_list_is_valid(&list));

    if (was_empty) {
        /* When the list is empty, pop_back returns NULL and the list is unchanged */
        assert(popped == NULL);
        assert(list.head.next == old.head.next);
        assert(list.tail.prev == old.tail.prev);
    } else {
        /* When the list is non‑empty, pop_back returns the former last node */
        assert(popped == old_last);
        /* The popped node's links are cleared */
        assert(popped->next == NULL);
        assert(popped->prev == NULL);
        /* The new last node is the former second‑last node */
        struct aws_linked_list_node *new_last = list.tail.prev;
        assert(new_last == old_last_prev);
        assert(new_last->next == &list.tail);
        /* Head of the list is unchanged */
        assert(list.head.next == old_first);
    }
}
