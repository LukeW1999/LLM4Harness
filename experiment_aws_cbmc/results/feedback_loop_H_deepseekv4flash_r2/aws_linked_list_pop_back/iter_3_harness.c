#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    /* Initialize a linked list on the stack */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Create a node on the stack and add it to the list */
    struct aws_linked_list_node node;
    aws_linked_list_push_back(&list, &node);

    /* Precondition: list is not empty */
    assert(!aws_linked_list_empty(&list));

    /* Call the function under verification */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    /* Postcondition: the popped node is the one we added */
    assert(popped == &node);

    /* Postcondition: the list is now empty */
    assert(aws_linked_list_empty(&list));
}
