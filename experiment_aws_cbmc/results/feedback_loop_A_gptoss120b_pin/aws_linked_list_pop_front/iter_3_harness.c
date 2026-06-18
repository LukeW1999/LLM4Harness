#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_first = list.head.next;

    struct aws_linked_list_node *removed = aws_linked_list_pop_front(&list);

    assert(aws_linked_list_is_valid(&list));

    if (old_first == &old.tail) {
        /* List was empty */
        assert(removed == NULL);
        assert(list.head.next == old.head.next);
        assert(list.tail.prev == old.tail.prev);
    } else {
        /* List had at least one element */
        assert(removed == old_first);
        assert(list.head.next == old_first->next);
        if (list.head.next != &list.tail) {
            assert(list.head.next->prev == &list.head);
        }
        if (old.tail.prev == old_first) {
            /* Only one element originally, now empty */
            assert(list.head.next == &list.tail);
            assert(list.tail.prev == &list.head);
        } else {
            /* More than one element originally */
            assert(list.tail.prev == old.tail.prev);
        }
    }

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
