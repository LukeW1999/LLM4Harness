/*  
 * Preconditions:  
 *   - `list` is a non‑NULL pointer to a `struct aws_linked_list`.  
 *   - The list satisfies `aws_linked_list_is_valid(list)` and  
 *     `aws_linked_list_is_valid_deep(list)`.  
 *   - The allocator used for any node allocation is `aws_default_allocator()`.  
 *   
 * Postconditions (validity):  
 *   - The returned pointer `rval` is equal to `list->tail.prev`.  
 *   - `rval` is either the list head sentinel (when the list is empty) or a node that is  
 *     currently linked in `list` (i.e., `aws_linked_list_node_is_in_list(rval)` is true).  
 *   
 * Postconditions (frame):  
 *   - The list structure (`head`, `tail`, and all nodes) is unchanged after the call.  
 *   - No memory is allocated or freed by `aws_linked_list_back`.  
 */ 

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_back_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Build a list of nondeterministic length (including empty). */
    size_t max_nodes = nondet_uint();
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

    /* Capture a snapshot of the list structure for frame checking. */
    struct aws_linked_list old_list = list;

    /* Call the function under verification. */
    struct aws_linked_list_node *rval = aws_linked_list_back(&list);

    /* Postcondition: return value equals tail.prev. */
    assert(rval == list.tail.prev);

    /* Postcondition: if the list is empty, tail.prev points to the head sentinel. */
    if (aws_linked_list_empty(&list)) {
        assert(rval == &list.head);
    } else {
        /* Otherwise, the returned node must be in the list. */
        assert(aws_linked_list_node_is_in_list(rval));
        /* Its next pointer must be the tail sentinel. */
        assert(rval->next == &list.tail);
    }

    /* Frame condition: the list structure is unchanged. */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* Deep validity must still hold after the call. */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
