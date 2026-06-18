#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node before_node;
    struct aws_linked_list_node prev_node;
    struct aws_linked_list_node to_add_node;

    /* Ensure nodes are distinct to avoid aliasing issues */
    __CPROVER_assume(&before_node != &to_add_node);
    __CPROVER_assume(&prev_node != &to_add_node);
    __CPROVER_assume(&before_node != &prev_node);

    /* before_node is in a list, so its prev points to prev_node, 
       and prev_node's next points to before_node */
    before_node.prev = &prev_node;
    prev_node.next = &before_node;

    /* Save old state for unchanged fields */
    struct aws_linked_list_node *old_before_next = before_node.next;

    /* Call function under test */
    aws_linked_list_insert_before(&before_node, &to_add_node);

    /* 1. Changed fields (from Doxygen: "Inserts to_add immediately before before") */
    assert(to_add_node.next == &before_node);
    assert(to_add_node.prev == &prev_node);
    assert(prev_node.next == &to_add_node);
    assert(before_node.prev == &to_add_node);

    /* 2. Unchanged fields */
    assert(before_node.next == old_before_next);

    /* 3. Validity invariants */
    /* prev_node's next is to_add_node, and to_add_node's prev is prev_node */
    assert(aws_linked_list_node_next_is_valid(&prev_node));
    /* to_add_node's next is before_node, and before_node's prev is to_add_node */
    assert(aws_linked_list_node_next_is_valid(&to_add_node));
    /* before_node's prev is to_add_node, and to_add_node's next is before_node */
    assert(aws_linked_list_node_prev_is_valid(&before_node));
    /* to_add_node's prev is prev_node, and prev_node's next is to_add_node */
    assert(aws_linked_list_node_prev_is_valid(&to_add_node));
}
