#include <aws/common/allocator.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    (void)allocator;

    struct aws_linked_list list;
    struct aws_linked_list_node node;

    aws_linked_list_init(&list);

    node.next = NULL;
    node.prev = NULL;

    assert(!aws_linked_list_node_is_in_list(&node));

    aws_linked_list_push_back(&list, &node);

    assert(aws_linked_list_node_is_in_list(&node));

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(!aws_linked_list_node_is_in_list(&node));
}
