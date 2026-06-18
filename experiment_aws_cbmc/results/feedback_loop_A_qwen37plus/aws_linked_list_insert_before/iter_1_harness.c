#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    // Satisfy the requirement to initialize a linked list environment
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    // We model a local segment of a linked list to represent the 'before' node's context.
    // This is more general and avoids complex reachability assumptions.
    struct aws_linked_list_node node_before;
    struct aws_linked_list_node node_to_add;
    struct aws_linked_list_node node_prev;
    struct aws_linked_list_node node_next;

    struct aws_linked_list_node *before = &node_before;
    struct aws_linked_list_node *to_add = &node_to_add;

    // Ensure all relevant nodes are distinct to avoid aliasing issues that could 
    // corrupt the list unexpectedly (e.g., inserting a node before itself).
    __CPROVER_assume(before != to_add);
    __CPROVER_assume(before != &node_prev);
    __CPROVER_assume(before != &node_next);
    __CPROVER_assume(to_add != &node_prev);
    __CPROVER_assume(to_add != &node_next);
    // Note: We do NOT assume &node_prev != &node_next, because they can be the same 
    // (e.g., in an empty list, head->prev is tail, and head->next is tail).

    // Setup a valid local context for `before`.
    // `before` must have a valid `prev` node that points back to it.
    before->prev = &node_prev;
    node_prev.next = before;
    
    // Setup `before->next` to be something, though it's not modified by the function.
    before->next = &node_next;
    node_next.prev = before;

    // Save old state for unchanged fields
    struct aws_linked_list_node *old_before_next = before->next;
    struct aws_linked_list_node *old_prev_prev = node_prev.prev;
    struct aws_linked_list_node *old_next_prev = node_next.prev;

    // Call function under test
    aws_linked_list_insert_before(before, to_add);

    // 1. Assert changed fields (from Doxygen and implementation)
    assert(to_add->next == before);
    assert(to_add->prev == &node_prev);
    assert(before->prev->next == to_add);
    assert(before->prev == to_add);

    // 2. Assert unchanged fields
    assert(before->next == old_before_next);
    assert(node_prev.prev == old_prev_prev);
    assert(node_next.prev == old_next_prev);
    
    // 3. Validity invariants
    // The new connections are bidirectional
    assert(to_add->next->prev == to_add);
    assert(to_add->prev->next == to_add);
}
