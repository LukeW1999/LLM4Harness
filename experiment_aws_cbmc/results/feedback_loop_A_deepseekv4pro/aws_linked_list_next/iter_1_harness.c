#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    struct aws_linked_list old_list = list;

    const struct aws_linked_list_node *node = &list.head;
    struct aws_linked_list_node old_node = *node;

    struct aws_linked_list_node *result = aws_linked_list_next(node);

    assert(result == old_node.next);
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(aws_linked_list_is_valid_deep(&list));
}
