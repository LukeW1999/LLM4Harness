#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    if (nondet_bool()) {
        struct aws_linked_list_node node;
        aws_linked_list_node_init(&node);
        aws_linked_list_push_back(&list, &node);
        aws_linked_list_pop_back(&list);
        assert(aws_linked_list_is_empty(&list));
        assert(!aws_linked_list_node_is_in_list(&node));
    } else {
        aws_linked_list_pop_back(&list);
        assert(aws_linked_list_is_empty(&list));
    }
}
