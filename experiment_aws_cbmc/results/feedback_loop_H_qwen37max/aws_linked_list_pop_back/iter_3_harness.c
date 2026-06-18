#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *back_node = list.tail.prev;
    struct aws_linked_list_node *prev_node = back_node->prev;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result == back_node);
    assert(list.tail.prev == prev_node);
    assert(prev_node->next == &list.tail);
}
