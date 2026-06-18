/*  
 * Preconditions:  
 *   - `list` is a non‑NULL pointer to a `struct aws_linked_list`.  
 *   - The list satisfies `aws_linked_list_is_valid(list)` and  
 *     `aws_linked_list_is_valid_deep(list)`.  
 *   - The list is non‑empty (required by `aws_linked_list_back`).  
 *   
 * Postconditions (validity):  
 *   - The returned pointer `rval` equals `list.tail.prev`.  
 *   - `rval` is a node that is linked in `list` (`aws_linked_list_node_is_in_list(rval)` is true).  
 *   
 * Postconditions (frame):  
 *   - The list structure (`head`, `tail`, and all nodes) is unchanged after the call.  
 *   - No memory is allocated or freed by `aws_linked_list_back`.  
 */ 

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_back_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Build a list of nondeterministic length (at least one node). */
    size_t max_nodes = nondet_uint();
    __CPROVER_assume(max_nodes >= 1);
    __CPROVER_assume(max_nodes <= 10); /* bound to keep verification tractable */

    for (size_t i = 0; i < max_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);
    }

    /* Assume the list is valid before the call. */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Capture a snapshot of the list structure for frame checking. */
    struct aws_linked_list old_list = list;

    /* Call the function under verification. */
    struct aws_linked_list_node *rval = aws_linked_list_back(&list);

    /* Postcondition: return value equals tail.prev. */
    assert(rval == list.tail.prev);

    /* The returned node must be in the list and its next must be the tail sentinel. */
    assert(aws_linked_list_node_is_in_list(rval));
    assert(rval->next == &list.tail);

    /* Frame condition: the list structure is unchanged. */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* Deep validity must still hold after the call. */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
