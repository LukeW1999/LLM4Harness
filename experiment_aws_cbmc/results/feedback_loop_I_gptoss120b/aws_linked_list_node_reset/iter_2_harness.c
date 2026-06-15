#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_node_reset_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* allocate a node */
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)aws_mem_acquire(alloc, sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* give the node nondeterministic prev/next pointers (may be NULL or any address) */
    node->next = (struct aws_linked_list_node *)nondet_uintptr();
    node->prev = (struct aws_linked_list_node *)nondet_uintptr();

    /* call function under test */
    aws_linked_list_node_reset(node);

    /* post‑conditions: node is reset to point to itself */
    assert(node->next == node);
    assert(node->prev == node);

    /* frame condition: allocator pointer unchanged */
    assert(alloc == aws_default_allocator());
}
