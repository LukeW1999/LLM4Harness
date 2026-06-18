#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_prev_harness(void) {
    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate two nodes */
    struct aws_linked_list_node *node = aws_mem_acquire(allocator, sizeof *node);
    struct aws_linked_list_node *prev_node = aws_mem_acquire(allocator, sizeof *prev_node);
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(prev_node != NULL);

    /* Build a minimal valid list fragment */
    node->prev = prev_node;
    node->next = NULL;               /* not used by aws_linked_list_prev */
    prev_node->next = node;
    prev_node->prev = NULL;          /* can be NULL, not required for the checks */

    /* Save old state of the node */
    struct aws_linked_list_node old = *node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postconditions */
    assert(result == old.prev);      /* returned pointer unchanged */
    assert(node->prev == old.prev);  /* node fields unchanged */
    assert(node->next == old.next);
}
