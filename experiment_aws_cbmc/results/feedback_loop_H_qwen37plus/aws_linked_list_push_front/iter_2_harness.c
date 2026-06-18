#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    
    aws_linked_list_init(&list);
    aws_linked_list_push_front(&list, &node);
    
    assert(list.head.next == &node);
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == &list.head);
}
