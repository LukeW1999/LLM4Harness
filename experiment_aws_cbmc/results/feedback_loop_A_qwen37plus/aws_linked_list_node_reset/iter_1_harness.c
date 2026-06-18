#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node *node_ptr = &node;

    aws_linked_list_node_reset(node_ptr);

    assert(node_ptr->next == NULL);
    assert(node_ptr->prev == NULL);
}
