#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *old_first = list.head.next;
    bool was_empty = (old_first == &list.tail);

    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    assert(aws_linked_list_is_valid(&list));

    if (was_empty) {
        assert(popped == NULL);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        assert(popped == old_first);
        assert(list.head.next->prev == &list.head);
        if (list.head.next == &list.tail) {
            /* List became empty */
            assert(list.tail.prev == &list.head);
        } else {
            assert(list.head.next != &list.tail);
        }
    }
}
