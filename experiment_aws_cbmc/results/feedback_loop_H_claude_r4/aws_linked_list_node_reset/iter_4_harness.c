#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_node_reset_harness(void) {
    /* 1. Set up the node - allocate on stack, no need for malloc */
    struct aws_linked_list_node node;

    /* Initialize with nondet values to ensure reset actually changes them */
    struct aws_linked_list_node *nondet_next;
    struct aws_linked_list_node *nondet_prev;
    node.next = nondet_next;
    node.prev = nondet_prev;

    /* 2. Call function under test */
    aws_linked_list_node_reset(&node);

    /* 3. Assert postconditions */
    /* After reset, next and prev should be NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
