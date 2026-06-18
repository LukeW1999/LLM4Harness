#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    struct aws_linked_list_node *node;

    if (nondet_bool()) {
        node = &list.tail;
    } else {
        __CPROVER_assume(!aws_linked_list_empty(&list));
        node = list.tail.prev;
    }

    __CPROVER_assume(AWS_MEM_IS_READABLE(node, sizeof(*node)));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    assert(result == old_prev);
    assert(result == old_node.prev);
    assert(result->next == node);

    assert(node->prev == old_prev);
    assert(node->next == old_next);
    assert(node->prev == old_node.prev);
    assert(node->next == old_node.next);

    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
