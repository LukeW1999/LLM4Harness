#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *node = aws_linked_list_begin(&list);

    __CPROVER_assume(node != aws_linked_list_end(&list));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(node, sizeof(*node)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(node->prev, sizeof(*node->prev)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(node->next, sizeof(*node->next)));
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_next_next = old_next->next;
    struct aws_linked_list_node *old_prev_prev = old_prev->prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    aws_linked_list_remove(node);

    assert(node->next == NULL);
    assert(node->prev == NULL);

    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    assert(old_prev->prev == old_prev_prev);
    assert(old_next->next == old_next_next);

    assert(list.head.next == old_next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    if (old_next == &list.tail) {
        assert(list.tail.prev == &list.head);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    assert(!aws_linked_list_node_is_in_list(node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
