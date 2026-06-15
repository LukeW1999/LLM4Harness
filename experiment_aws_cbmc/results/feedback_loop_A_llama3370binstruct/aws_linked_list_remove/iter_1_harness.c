#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_remove_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node prev_node;
    struct aws_linked_list_node next_node;

    __CPROVER_assume(prev_node.next == &node);
    __CPROVER_assume(node.prev == &prev_node);
    __CPROVER_assume(node.next == &next_node);
    __CPROVER_assume(next_node.prev == &node);

    struct aws_linked_list_node old_node = node;

    aws_linked_list_remove(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(prev_node.next == &next_node);
    assert(next_node.prev == &prev_node);

    assert(old_node.next != node.next);
    assert(old_node.prev != node.prev);
}
