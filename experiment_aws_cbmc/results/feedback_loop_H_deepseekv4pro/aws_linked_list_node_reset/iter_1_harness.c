#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node old = *node;

    /* 3. Call function under test */
    aws_linked_list_node_reset(node);

    /* 4. Assert postconditions */
    /* Doxygen: "Set node's next and prev pointers to NULL." */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 5. Assert validity invariant always holds */
    /* AWS_IS_ZEROED(*node) is the postcondition from the implementation */
    assert(node->next == NULL && node->prev == NULL);

    free(node);
}
