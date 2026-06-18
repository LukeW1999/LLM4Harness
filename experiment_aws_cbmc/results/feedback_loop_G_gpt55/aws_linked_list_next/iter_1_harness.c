#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    struct aws_linked_list_node *node = &list.head;

    for (size_t i = 0; i < MAX_LINKED_LIST_ITEM_ALLOCATION; ++i) {
        if (!nondet_bool()) {
            break;
        }
        if (node->next == &list.tail) {
            break;
        }
        node = node->next;
    }

    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node old_next_node = *old_next;

    struct aws_linked_list_node *result = aws_linked_list_next(node);

    assert(result == old_next);
    assert(result == old_node.next);
    assert(result != NULL);
    assert(result->prev == node);

    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    assert(old_next->next == old_next_node.next);
    assert(old_next->prev == old_next_node.prev);

    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
