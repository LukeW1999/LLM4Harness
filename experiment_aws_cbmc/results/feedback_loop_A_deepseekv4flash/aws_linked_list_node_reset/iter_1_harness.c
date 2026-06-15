#include <aws/common/linked_list.h>
#include <stddef.h>

void aws_linked_list_node_reset_harness() {
    /* Declare a node on the stack to ensure it is writable */
    struct aws_linked_list_node node;

    /* Non-deterministic initial values for next and prev */
    node.next = nondet_bool() ? NULL : malloc(sizeof(struct aws_linked_list_node));
    node.prev = nondet_bool() ? NULL : malloc(sizeof(struct aws_linked_list_node));

    /* Precondition: node pointer is not NULL */
    __CPROVER_assume(node.next != NULL || node.prev != NULL); /* ensure at least one pointer is non-null to test reset */

    /* Call the function under test */
    aws_linked_list_node_reset(&node);

    /* Postcondition: both pointers are NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);

    /* The entire struct is zeroed (implied by AWS_ZERO_STRUCT) */
    /* Since the struct only contains two pointers, the above assertions suffice */
}
