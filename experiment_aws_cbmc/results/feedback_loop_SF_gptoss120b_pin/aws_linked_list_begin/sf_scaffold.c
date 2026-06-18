#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_begin_harness(void) {
    /* Symbolic input: a linked list */
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* Assume the list satisfies its invariants */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* PRE-CALL SNAPSHOT */
    struct aws_linked_list_node *orig_head_next = list->head.next;
    struct aws_linked_list_node *orig_head_prev = list->head.prev;
    struct aws_linked_list_node *orig_tail_next = list->tail.next;
    struct aws_linked_list_node *orig_tail_prev = list->tail.prev;

    /* Call the function under verification */
    struct aws_linked_list_node *result = aws_linked_list_begin(list);

    /* ASSERT_POSTCONDITIONS_HERE */
}
