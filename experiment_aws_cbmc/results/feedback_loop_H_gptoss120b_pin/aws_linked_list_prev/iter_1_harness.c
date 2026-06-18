#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_linked_list_prev_harness(void) {
    /* Allocate a node */
    struct aws_linked_list_node *node = malloc(sizeof *node);
    __CPROVER_assume(node != NULL);

    /* Allocate a possible previous node (can be NULL as well) */
    struct aws_linked_list_node *prev_node = malloc(sizeof *prev_node);
    __CPROVER_assume(prev_node != NULL);
    /* Nondeterministically decide if prev is NULL */
    __CPROVER_assume(nondet_bool() ? (prev_node = NULL) : 1);

    /* Initialize the node fields nondeterministically */
    node->prev = prev_node;
    node->next = NULL; /* value not relevant for this function */

    /* Save old state */
    struct aws_linked_list_node old = *node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postconditions */
    /* The function should return the previous pointer unchanged */
    assert(result == old.prev);

    /* No fields of the node should be modified */
    assert(node->prev == old.prev);
    assert(node->next == old.next);
}
