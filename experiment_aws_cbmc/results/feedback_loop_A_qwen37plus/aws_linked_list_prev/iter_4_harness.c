#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node *node = can_fail_malloc(sizeof(*node));
    struct aws_linked_list_node *prev = can_fail_malloc(sizeof(*prev));
    __CPROVER_assume(node != NULL && prev != NULL);
    node->prev = prev;
    prev->next = node;

    struct aws_linked_list_node *rval = aws_linked_list_prev(node);
    
    assert(rval == node->prev);
}
