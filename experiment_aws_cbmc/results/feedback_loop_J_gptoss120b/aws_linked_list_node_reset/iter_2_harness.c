#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* Branch: node may have arbitrary (possibly non‑NULL) links or be already NULL */
    if (__CPROVER_nondet_bool()) {
        /* Allocate dummy nodes to serve as non‑NULL pointers */
        node.next = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
        node.prev = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));

        /* The dummy nodes themselves can contain arbitrary data */
        if (node.next) {
            node.next->next = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();
            node.next->prev = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();
        }
        if (node.prev) {
            node.prev->next = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();
            node.prev->prev = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();
        }
    } else {
        node.next = NULL;
        node.prev = NULL;
    }

    /* Call the function under test */
    aws_linked_list_node_reset(&node);

    /* Post‑conditions: the node must be fully reset */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
