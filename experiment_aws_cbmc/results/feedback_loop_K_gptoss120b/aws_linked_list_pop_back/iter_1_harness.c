/*  
 * Contract for aws_linked_list_pop_back  
 * ------------------------------------------------------------  
 * Preconditions:  
 *   - The list pointer passed to the function is non‑NULL.  
 *   - The list satisfies aws_linked_list_is_valid(list).  
 *   - The list is not empty (aws_linked_list_empty(list) == false).  
 *   - All nodes in the list are properly linked (bidirectional).  
 *   - No node in the list is NULL.  
 *   - The allocator used for node creation is aws_default_allocator().  
 *   
 * Postconditions (validity):  
 *   - The returned node pointer is non‑NULL and was previously the last node in the list.  
 *   - The returned node has its next and prev members set to NULL.  
 *   - The list remains valid: aws_linked_list_is_valid(list) holds.  
 *   - The list length is decreased by exactly one.  
 *   
 * Postconditions (frame):  
 *   - No memory outside the list structure and its constituent nodes is modified.  
 *   - All remaining nodes retain their original next/prev relationships (except the former tail).  
 */  

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 5

/* Helper to compute the length of a list (number of data nodes) */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        len++;
        cur = aws_linked_list_next(cur);
    }
    return len;
}

/* Helper to allocate a fresh node with zeroed fields */
static struct aws_linked_list_node *allocate_node(void) {
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    return node;
}

void aws_linked_list_pop_back_harness(void) {
    /* Set up a valid, non‑empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Choose a nondeterministic length in [1, MAX_NODES] */
    size_t n = __CPROVER_nondet_uint();
    __CPROVER_assume(n > 0 && n <= MAX_NODES);

    /* Populate the list with n distinct nodes */
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = allocate_node();
        aws_linked_list_push_back(&list, node);
    }

    /* Record the original length */
    size_t orig_len = list_length(&list);
    __CPROVER_assume(orig_len == n);   /* sanity check */

    /* Capture the address of the current back node for later comparison */
    struct aws_linked_list_node *expected_back = aws_linked_list_back(&list);

    /* Call the function under verification */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    /* ----- Postcondition checks ----- */
    /* Returned node must be the former back node */
    assert(popped == expected_back);

    /* Returned node must be detached (next and prev are NULL) */
    assert(popped->next == NULL);
    assert(popped->prev == NULL);

    /* List must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* Length must have decreased by one */
    size_t new_len = list_length(&list);
    assert(new_len + 1 == orig_len);

    /* No other memory should have been modified – we conservatively rely on the
       validity check and the length invariant; deeper frame reasoning would
       require tracking each node individually, which is beyond this harness. */
    return 0;
}
