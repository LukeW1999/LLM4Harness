#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *second_node = old_front->next;

    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(popped == old_front);
    assert(list.head.next == second_node);
    assert(second_node->prev == &list.head);
}
