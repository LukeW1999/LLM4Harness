#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    struct aws_linked_list_node after;
    struct aws_linked_list_node next_node;
    struct aws_linked_list_node to_add;

    /* Ensure nodes are distinct to test the standard insertion case */
    __CPROVER_assume(&after != &next_node);
    __CPROVER_assume(&after != &to_add);
    __CPROVER_assume(&next_node != &to_add);

    /* Setup initial valid state for the "after" and "next" connection.
     * after->next must be a valid node because the function dereferences it. */
    after.next = &next_node;
    next_node.prev = &after;

    /* Save old state for unchanged field checks */
    struct aws_linked_list_node *old_after_prev = after.prev;
    struct aws_linked_list_node *old_next_next = next_node.next;

    /* Call function under test */
    aws_linked_list_insert_after(&after, &to_add);

    /* 1. Changed fields (from Doxygen: "Inserts to_add immediately after after") */
    assert(to_add.prev == &after);
    assert(to_add.next == &next_node);
    assert(next_node.prev == &to_add);
    assert(after.next == &to_add);

    /* 2. Unchanged fields */
    assert(after.prev == old_after_prev);
    assert(next_node.next == old_next_next);

    /* 3. Validity invariants */
    /* The connection between after and to_add is bidirectional */
    assert(aws_linked_list_node_next_is_valid(&after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    
    /* The connection between to_add and next_node is bidirectional */
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&next_node));
}
