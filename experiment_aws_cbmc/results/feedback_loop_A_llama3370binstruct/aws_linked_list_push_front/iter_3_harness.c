#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    aws_linked_list_init(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));

    struct aws_linked_list_node node;
    ensure_linked_list_node_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_valid(&node));

    bool success = aws_linked_list_push_front(&list, &node);
    assert(success);

    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == &list.tail);
    assert(list.tail.prev == &node);

    struct aws_linked_list_node node2;
    ensure_linked_list_node_is_allocated(&node2, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_valid(&node2));

    success = aws_linked_list_push_front(&list, &node2);
    assert(success);

    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));
    assert(list.head.next == &node2);
    assert(node2.prev == &list.head);
    assert(node2.next == &node);
    assert(node.prev == &node2);
    assert(node.next == &list.tail);
    assert(list.tail.prev == &node);

    aws_linked_list_clean_up(&list);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));
}

int main() {
    aws_linked_list_push_front_harness();
    return 0;
}
