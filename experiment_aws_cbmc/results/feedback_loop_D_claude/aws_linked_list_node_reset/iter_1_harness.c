// === STEP 1: SUCCESS PATH ===
// aws_linked_list_node_reset has no return value (void).
// On success (always, given non-NULL node):
//   - node->next: CHANGES to NULL (zeroed)
//   - node->prev: CHANGES to NULL (zeroed)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and only has a precondition (node != NULL).
// If node is NULL, precondition fails (undefined behavior / assertion).
//
// === STEP 3: FRAME CONDITIONS ===
// node (struct aws_linked_list_node *):
//   - next: CHANGED to NULL
//   - prev: CHANGED to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
// After the call:
//   - node->next == NULL
//   - node->prev == NULL
//   - AWS_IS_ZEROED(*node) holds (both fields are NULL/zero)

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    // Allocate a non-deterministic linked list node
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    
    // Precondition: node must not be NULL
    __CPROVER_assume(node != NULL);
    
    // node->next and node->prev can be anything (non-deterministic)
    // They are already non-deterministic from malloc
    
    // Call the function under test
    aws_linked_list_node_reset(node);
    
    // === POST-CONDITIONS ===
    
    // STEP 1: Verify node fields are zeroed after the call
    assert(node->next == NULL);
    assert(node->prev == NULL);
    
    // STEP 4: Verify the AWS_IS_ZEROED postcondition
    // AWS_IS_ZEROED checks that the struct is all zeros
    // Since the struct only has next and prev, checking both NULL is sufficient
    assert(node->next == NULL && node->prev == NULL);
}
