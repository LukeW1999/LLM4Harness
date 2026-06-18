#include <assert.h>
#include <stdlib.h>
#include <aws/common/allocator.h>
#include <aws/common/linked_list.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* Allocate a zero‑initialized node */
    struct aws_linked_list_node *node =
        aws_mem_calloc(alloc, 1, sizeof(struct aws_linked_list_node));

    /* Assume allocation succeeded for the purpose of this harness */
    __CPROVER_assume(node != NULL);

    aws_linked_list_node_reset(node);

    assert(node->next == node);
    assert(node->prev == node);

    aws_mem_release(alloc, node);
}
