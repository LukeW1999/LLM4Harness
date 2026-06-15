#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* Allocate a single node on the stack and non-deterministically initialize its contents */
    struct aws_linked_list_node node;
    __CPROVER_havoc_object(&node);

    /* Save the old state of the node */
    struct aws_linked_list_node old_node = node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* Postconditions */
    assert(result == node.next);   // The function returns node->next
    assert(node.next == old_node.next); // The node itself is not modified (const)
    assert(node.prev == old_node.prev);
}
