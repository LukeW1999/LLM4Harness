#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    
    struct aws_linked_list_node node;
    aws_linked_list_push_back(&list, &node);
    
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);
    assert(popped == &node);
    assert(aws_linked_list_empty(&list));
}
