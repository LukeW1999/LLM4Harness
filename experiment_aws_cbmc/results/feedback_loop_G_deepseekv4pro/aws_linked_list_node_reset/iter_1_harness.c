#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    /* node is uninitialised, but it must be a valid, writeable memory location.
     * Its address is passed to the function, satisfying the precondition node != NULL.
     */
    aws_linked_list_node_reset(&node);

    /* Postcondition: next and prev are both NULL (as per Doxygen). */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
