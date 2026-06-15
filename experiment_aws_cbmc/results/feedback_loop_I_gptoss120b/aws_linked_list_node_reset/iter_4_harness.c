#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_linked_list_node *node =
        (struct aws_linked_list_node *)aws_mem_acquire(alloc, sizeof(*node));
    __CPROVER_assume(node != NULL);

    aws_linked_list_node_reset(node);

    assert(node->next == NULL);
    assert(node->prev == NULL);
}
