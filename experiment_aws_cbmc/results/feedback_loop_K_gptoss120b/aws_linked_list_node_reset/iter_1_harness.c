/*=== Contract for aws_linked_list_node_reset ===
Preconditions:
  - The pointer `node` must be non‑NULL and point to a valid, writable
    `struct aws_linked_list_node` object.

Postconditions (validity):
  - After the call, `node->next == NULL` and `node->prev == NULL`.
  - The whole structure is zeroed (`AWS_IS_ZEROED(*node)`).

Postconditions (frame):
  - No memory other than the object pointed to by `node` is modified.
===*/

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness(void) {
    /* Allocate a node and give it nondeterministic contents */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    /* Fill with nondet values */
    node->next = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    node->prev = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(node->prev != NULL);

    /* Frame: allocate a dummy object that must stay unchanged */
    int dummy = nondet_int();
    int dummy_before = dummy;

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Verify postconditions */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    /* Verify the whole struct is zeroed */
    struct aws_linked_list_node zero = {0};
    assert(node->next == zero.next);
    assert(node->prev == zero.prev);

    /* Verify frame condition */
    assert(dummy == dummy_before);

    /* Clean up */
    free(node->next);
    free(node->prev);
    free(node);
    return 0;
}
