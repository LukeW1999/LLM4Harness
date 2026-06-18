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
    /* list pointer must remain non‑NULL */
    assert(list != NULL);

    /* The list must still be a valid linked list */
    assert(aws_linked_list_is_valid(list));

    /* The tail.prev field must be unchanged */
    assert(list->tail.prev == orig_tail_prev);

    /* The returned pointer is either NULL (empty list) or the original tail.prev */
    assert(result == NULL || result == orig_tail_prev);

    /* If a node is returned, it must be the last node in the list */
    if (result != NULL) {
        assert(result->next == &list->tail);
        /* The previous node of the returned node must be the node that preceded
           the original tail.prev (or the list head if the list had a single element). */
        if (orig_tail_prev != &list->head) {
            assert(result->prev == orig_tail_prev->prev);
        } else {
            assert(result->prev == &list->head);
        }
    }
}
