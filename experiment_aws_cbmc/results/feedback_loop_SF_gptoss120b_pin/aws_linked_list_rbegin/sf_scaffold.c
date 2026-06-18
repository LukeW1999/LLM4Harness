#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness(void) {
    /* Symbolic input: linked list */
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* Initialize list (could be empty or populated later) */
    aws_linked_list_init(list);

    /* Optionally nondeterministically populate the list */
    /* (populate logic can be added here if desired) */

    /* Precondition: list must be a valid linked list */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Snapshot of relevant state before the call */
    struct aws_linked_list_node *orig_tail_prev = list->tail.prev;

    /* Call the function under verification */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(list);

    /* ASSERT_POSTCONDITIONS_HERE */
}
