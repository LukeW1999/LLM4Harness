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

    /* Precondition: list must be a valid linked list */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Snapshot of relevant state before the call */
    struct aws_linked_list_node *orig_tail_prev = list->tail.prev;

    /* Call the function under verification */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(list);

    /* Post‑conditions */
    assert(list != NULL);
    assert(aws_linked_list_is_valid(list));
    assert(list->tail.prev == orig_tail_prev);
    assert(result == NULL || result == orig_tail_prev);

    if (result != NULL) {
        /* The returned node must be the last node in the list */
        assert(result->next == &list->tail);
        /* For a non‑empty list the previous node of the returned node is the node
           that preceded the original tail.prev (or the list head if there was only
           one element). */
        if (orig_tail_prev != &list->head) {
            assert(result->prev == orig_tail_prev->prev);
        }
    }
}
