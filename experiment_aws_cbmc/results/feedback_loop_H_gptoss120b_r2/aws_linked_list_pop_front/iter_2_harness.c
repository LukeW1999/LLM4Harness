#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_first = list.head.next;

    /* Call function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* Post‑condition: list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Sentinel fields must stay correct */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    if (aws_linked_list_empty(&old_list)) {
        /* No element to pop */
        assert(popped == NULL);
        /* List unchanged */
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
    } else {
        /* An element was removed */
        assert(popped != NULL);
        /* Removed node's links are cleared */
        assert(popped->next == NULL);
        assert(popped->prev == NULL);

        /* New first element */
        struct aws_linked_list_node *new_first = old_first->next;
        assert(list.head.next == new_first);
        assert(new_first->prev == &list.head);

        if (old_first->next == &old_list.tail) {
            /* List had a single element, now empty */
            assert(aws_linked_list_empty(&list));
            assert(list.head.next == &list.tail);
            assert(list.tail.prev == &list.head);
        } else {
            /* List still non‑empty, tail unchanged */
            assert(list.tail.prev == old_list.tail.prev);
        }
    }
}
