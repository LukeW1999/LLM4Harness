#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_node_reset_harness(void) {
    /* Allocate a node */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* Nondeterministically decide whether the node is initially linked */
    bool linked = nondet_bool();

    if (linked) {
        /* Allocate and initialise a list */
        struct aws_linked_list list;
        ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
        __CPROVER_assume(aws_linked_list_is_valid(&list));

        /* Insert the node into the list (push to front) */
        aws_linked_list_push_front(&list, node);
    }

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Post‑conditions */
    assert(node->next == NULL);
    assert(node->prev == NULL);
}
