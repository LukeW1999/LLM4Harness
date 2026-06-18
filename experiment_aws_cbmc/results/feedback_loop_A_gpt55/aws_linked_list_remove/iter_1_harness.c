#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_remove_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *node = list.head.next;

    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head);
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(node, sizeof(*node)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(node->prev, sizeof(*node->prev)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(node->next, sizeof(*node->next)));

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev_prev = old_prev->prev;
    struct aws_linked_list_node *old_next_next = old_next->next;

    __CPROVER_assume(old_prev != NULL);
    __CPROVER_assume(old_next != NULL);
    __CPROVER_assume(old_prev != node);
    __CPROVER_assume(old_next != node);

    aws_linked_list_remove(node);

    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(!aws_linked_list_node_is_in_list(node));

    assert(old_prev->prev == old_prev_prev);
    assert(old_next->next == old_next_next);

    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    if (old_prev == &old_list.head) {
        assert(list.head.next == old_next);
    } else {
        assert(list.head.next == old_list.head.next);
    }

    if (old_next == &old_list.tail) {
        assert(list.tail.prev == old_prev);
    } else {
        assert(list.tail.prev == old_list.tail.prev);
    }

    if (old_prev == &old_list.head && old_next == &old_list.tail) {
        assert(aws_linked_list_empty(&list));
    } else {
        assert(!aws_linked_list_empty(&list));
    }

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
