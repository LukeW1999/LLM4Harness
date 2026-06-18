#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *node = aws_linked_list_begin(&list);

    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    aws_linked_list_remove(node);

    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_node_is_in_list(node));
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
