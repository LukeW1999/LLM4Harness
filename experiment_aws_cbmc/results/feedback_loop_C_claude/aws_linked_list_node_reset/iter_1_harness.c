// === STEP 1: SUCCESS PATH ===
// aws_linked_list_node_reset is void, no return value.
// On success (always, given non-NULL node):
//   - node->next: CHANGES to NULL (zeroed)
//   - node->prev: CHANGES to NULL (zeroed)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always succeeds given precondition (node != NULL)
//
// === STEP 3: FRAME CONDITIONS ===
// param: node (struct aws_linked_list_node *):
//   - node->next: CHANGED to NULL
//   - node->prev: CHANGED to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
// After call: node->next == NULL && node->prev == NULL (AWS_IS_ZEROED)
// The node is no longer in a list (both pointers NULL)

#include <aws/common/linked_list.h>
#include <stddef.h>

void aws_linked_list_node_reset_harness(void) {
    /* Allocate a node non-deterministically */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));

    /* Precondition: node must not be NULL */
    __CPROVER_assume(node != NULL);

    /* Give node non-deterministic initial values */
    node->next = nondet_bool() ? NULL : node; /* arbitrary pointer */
    node->prev = nondet_bool() ? NULL : node; /* arbitrary pointer */

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Postconditions: both pointers must be NULL (zeroed) */
    assert(node->next == NULL);
    assert(node->prev == NULL);
}
