#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    struct aws_linked_list_node node;
    node.next = &list.head;
    node.prev = &list.head;
    list.head.next = &node;
    list.head.prev = &node;

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    aws_linked_list_node_reset(&node);

    assert(node.next == &node);
    assert(node.prev == &node);
}
