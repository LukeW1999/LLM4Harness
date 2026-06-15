#include <aws/common/linked_list.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Post‑condition: all bytes of the node must be zero after reset */
    for (size_t i = 0; i < sizeof(*node); ++i) {
        assert(((uint8_t *)node)[i] == 0);
    }
}
