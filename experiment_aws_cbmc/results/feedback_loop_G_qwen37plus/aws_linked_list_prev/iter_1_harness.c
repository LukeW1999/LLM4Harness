#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(AWS_MEM_IS_READABLE(&node, sizeof(struct aws_linked_list_node)));

    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_prev = node.prev;

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    assert(result == old_prev);
    assert(node.next == old_next);
    assert(node.prev == old_prev);
}
