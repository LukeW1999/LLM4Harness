#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    const struct aws_linked_list_node *node = &list.tail;
    __CPROVER_assume(AWS_MEM_IS_READABLE(node, sizeof(*node)));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node = *node;

    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    assert(result == old_node.prev);

    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    assert(AWS_MEM_IS_READABLE(result, sizeof(*result)));
    assert(result->next == node);
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_is_valid(&list));
}
