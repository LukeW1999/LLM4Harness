#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    const size_t max_items = 5;
    ensure_linked_list_is_allocated(&list, allocator, max_items);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure the list is non‑empty so we have a real node to reset */
    __CPROVER_assume(list.head.next != &list.head);
    struct aws_linked_list_node *node = list.head.next;

    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);

    assert(node->next == node);
    assert(node->prev == node);
}
