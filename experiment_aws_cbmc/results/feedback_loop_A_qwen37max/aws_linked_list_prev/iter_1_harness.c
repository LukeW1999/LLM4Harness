#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node prev_node;
    
    node.prev = nondet_bool() ? &prev_node : NULL;
    node.next = NULL;
    
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);
    
    assert(result == node.prev);
}
