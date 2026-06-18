#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Nondeterministically decide how many nodes to put in the list (at least one) */
    uint32_t num_nodes = nondet_uint();
    __CPROVER_assume(num_nodes > 0);
    __CPROVER_assume(num_nodes < 5);

    /* Populate the list */
    for (uint32_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&list, node);
    }

    /* Preconditions for the function under test */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Snapshot of the original state */
    uint32_t original_len = 0;
    struct aws_linked_list_node *orig_back = NULL;
    struct aws_linked_list_node *it = list.head.next;
    while (it != &list.tail) {
        original_len++;
        orig_back = it;
        it = it->next;
    }

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    
}
