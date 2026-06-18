#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Initialize an empty list */
    aws_linked_list_init(&list);

    /* Add a single node to the back */
    aws_linked_list_push_back(&list, &node);

    /* Remove the node from the back */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    /* The popped pointer should refer to the node that was pushed */
    assert(popped == &node);

    /* The list must be empty after the removal */
    assert(aws_linked_list_empty(&list));

    /* The node's pointers should be reset to NULL by the library */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
