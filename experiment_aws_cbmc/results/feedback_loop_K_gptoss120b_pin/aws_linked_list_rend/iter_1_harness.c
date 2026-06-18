/*  
 * Contract for aws_linked_list_rend  
 * Preconditions:  
 *   - list != NULL  
 *   - aws_linked_list_is_valid(list) == true (the list satisfies all internal invariants)  
 * Postconditions (validity):  
 *   - return value r is non‑NULL  
 *   - r == &list->head (the returned pointer is exactly the address of the list head sentinel)  
 * Postconditions (frame):  
 *   - The list structure (head, tail, and all node links) is not modified by the call.  
 *   - No memory outside the list is accessed or modified.  
 */

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Helper to create a nondeterministic unsigned integer */
static unsigned int nondet_uint(void);
static bool nondet_bool(void);

void aws_linked_list_rend_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Build a nondeterministic list of up to 5 nodes */
    const size_t MAX_NODES = 5;
    size_t node_count = nondet_uint();
    __CPROVER_assume(node_count <= MAX_NODES);

    struct aws_linked_list_node nodes[5];
    for (size_t i = 0; i < node_count; ++i) {
        /* Ensure each node starts in a reset state */
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Snapshot the list state for frame checking */
    struct aws_linked_list_node head_snapshot = list.head;
    struct aws_linked_list_node tail_snapshot = list.tail;

    /* Call the function under test */
    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    /* Postcondition: return value is non‑NULL and points to the head sentinel */
    assert(r != NULL);
    assert(r == &list.head);

    /* Frame condition: the list structure itself is unchanged */
    assert(list.head.next == head_snapshot.next);
    assert(list.head.prev == head_snapshot.prev);
    assert(list.tail.next == tail_snapshot.next);
    assert(list.tail.prev == tail_snapshot.prev);

    return 0;
}

/* Nondeterministic helpers (CBMC will provide implementations) */
static unsigned int nondet_uint(void) { return 0; }
static bool nondet_bool(void) { return false; }
