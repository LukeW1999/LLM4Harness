#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    /* Precondition: node != NULL, satisfied by address of local variable */
    aws_linked_list_node_reset(&node);

    /* Postcondition: the whole node is zeroed (next and prev set to NULL) */
    assert(node.next == NULL);
    assert(node.prev == NULL);

    /* Verify that the entire struct is zeroed, as per AWS_IS_ZEROED postcondition */
    const unsigned char *bytes = (const unsigned char *)&node;
    for (size_t i = 0; i < sizeof(node); i++) {
        assert(bytes[i] == 0);
    }
}
