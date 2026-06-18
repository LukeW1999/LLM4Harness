#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_last_prev = old_last->prev;
    bool had_one_item = old_first == old_last;

    __CPROVER_assert(old_head_prev == NULL, "head prev is NULL");
    __CPROVER_assert(old_tail_next == NULL, "tail next is NULL");
    __CPROVER_assert(old_last != &list.head, "list is not empty");
    __CPROVER_assert(old_last->next == &list.tail, "old last points to tail");
    __CPROVER_assert(old_last_prev != NULL, "old last prev is not NULL");

    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    __CPROVER_assert(popped == old_last, "pop_back returns the old last node");
    __CPROVER_assert(popped->next == NULL, "popped node next is reset");
    __CPROVER_assert(popped->prev == NULL, "popped node prev is reset");

    __CPROVER_assert(list.tail.prev == old_last_prev, "tail prev is updated");
    __CPROVER_assert(old_last_prev->next == &list.tail, "new last points to tail");

    __CPROVER_assert(list.head.prev == old_head_prev, "head prev unchanged");
    __CPROVER_assert(list.tail.next == old_tail_next, "tail next unchanged");

    if (had_one_item) {
        __CPROVER_assert(old_last_prev == &list.head, "single item prev was head");
        __CPROVER_assert(list.head.next == &list.tail, "list is now empty");
        __CPROVER_assert(aws_linked_list_empty(&list), "list is empty after popping only item");
    } else {
        __CPROVER_assert(old_last_prev != &list.head, "multiple items remain");
        __CPROVER_assert(list.head.next == old_first, "first node unchanged");
        __CPROVER_assert(old_first->prev == &list.head, "first node prev unchanged");
        __CPROVER_assert(!aws_linked_list_empty(&list), "list is not empty");
    }

    __CPROVER_assert(aws_linked_list_node_prev_is_valid(popped), "popped prev is valid");
    __CPROVER_assert(aws_linked_list_node_next_is_valid(popped), "popped next is valid");
    __CPROVER_assert(!aws_linked_list_node_is_in_list(popped), "popped node is not in a list");
    __CPROVER_assert(aws_linked_list_is_valid(&list), "list is valid");
    __CPROVER_assert(aws_linked_list_is_valid_deep(&list), "list is deeply valid");
}
