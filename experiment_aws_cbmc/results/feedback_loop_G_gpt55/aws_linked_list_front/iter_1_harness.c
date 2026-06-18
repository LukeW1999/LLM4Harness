#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_front_next = old_front->next;
    struct aws_linked_list_node *old_front_prev = old_front->prev;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(result == old_front);
    assert(result == old.head.next);
    assert(result == list.head.next);
    assert(result != aws_linked_list_end(&list));
    assert(result->prev == &list.head);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    assert(old_front->next == old_front_next);
    assert(old_front->prev == old_front_prev);

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
