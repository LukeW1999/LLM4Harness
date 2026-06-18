#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after_node;
    struct aws_linked_list_node to_add_node;
    struct aws_linked_list_node next_node;

    // Setup valid next connection for 'after'
    after_node.next = &next_node;
    next_node.prev = &after_node;

    // Assume the 'after' node has a valid next connection (i.e., after->next != NULL and after->next->prev == after)
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&after_node));
    
    // Assume to_add is a distinct node to model typical insertion of a new node
    __CPROVER_assume(&to_add_node != &after_node);
    __CPROVER_assume(&to_add_node != &next_node);

    // Save old state for unchanged fields
    struct aws_linked_list_node *old_after_prev = after_node.prev;
    struct aws_linked_list_node *old_next_next = next_node.next;
    struct aws_linked_list_node *old_after_next = after_node.next;

    // Call function under test
    aws_linked_list_insert_after(&after_node, &to_add_node);

    // 1. Changed fields
    assert(to_add_node.prev == &after_node);
    assert(to_add_node.next == old_after_next);
    assert(old_after_next->prev == &to_add_node);
    assert(after_node.next == &to_add_node);

    // 2. Unchanged fields
    assert(after_node.prev == old_after_prev);
    assert(next_node.next == old_next_next);

    // 3. Validity invariants
    assert(aws_linked_list_node_next_is_valid(&after_node));
    assert(aws_linked_list_node_next_is_valid(&to_add_node));
    assert(aws_linked_list_node_prev_is_valid(&to_add_node));
    assert(aws_linked_list_node_prev_is_valid(old_after_next));
}
