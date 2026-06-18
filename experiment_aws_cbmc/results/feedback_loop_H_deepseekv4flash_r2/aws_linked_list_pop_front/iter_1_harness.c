#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    /* Non-deterministic initialization for node pointers */
    node.next = nondet_bool() ? malloc(sizeof(struct aws_linked_list_node)) : NULL;
    node.prev = nondet_bool() ? malloc(sizeof(struct aws_linked_list_node)) : NULL;

    /* Precondition: node != NULL */
    __CPROVER_assume(&node != NULL);

    /* Save old state */
    struct aws_linked_list_node old = node;

    /* Call function under test */
    aws_linked_list_node_reset(&node);

    /* Postcondition: node.next and node.prev should be NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);

    /* Unchanged: node's identity (address) is unchanged */
    assert(&node == &node);
}
