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

    struct aws_linked_list_node *node = ensure_linked_list_node_is_allocated();
    __CPROVER_assume(aws_linked_list_node_is_valid(node));

    bool result = aws_linked_list_push_front(&list, node);
    assert(result == true);

    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));
    assert(list.head.next == node);
    assert(node->prev == &list.head);
    assert(node->next == &list.tail);
    assert(list.tail.prev == node);

    aws_linked_list_clean_up(&list);
}

int main() {
    aws_linked_list_push_front_harness();
    return 0;
}
