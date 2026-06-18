#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node prev;
    
    node.prev = &prev;
    prev.next = &node;
    
    struct aws_linked_list_node *rval = aws_linked_list_prev(&node);
    
    __CPROVER_assert(aws_linked_list_node_prev_is_valid(&node), "aws_linked_list_node_prev_is_valid(node)");
    __CPROVER_assert(aws_linked_list_node_next_is_valid(rval), "aws_linked_list_node_next_is_valid(rval)");
    __CPROVER_assert(rval == &prev, "rval == prev");
}
