#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

#include <stdbool.h>
#include <assert.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    node.next = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();
    node.prev = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();

    aws_linked_list_node_reset(&node);

    assert(node.next == &node);
    assert(node.prev == &node);
}
