#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node *node;
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    struct aws_linked_list_node *rval = aws_linked_list_prev(node);
    
    assert(rval == node->prev);
}
