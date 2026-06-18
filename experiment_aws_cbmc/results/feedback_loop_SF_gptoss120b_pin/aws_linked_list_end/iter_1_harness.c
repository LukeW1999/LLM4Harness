#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_end_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* PRE-CALL SNAPSHOT */
    const struct aws_linked_list_node *pre_tail = &list->tail;

    /* CALL FUNCTION */
    const struct aws_linked_list_node *result = aws_linked_list_end(list);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* The function must return a non‑NULL pointer */
    assert(result != NULL);
    /* The returned pointer must be the address of the list's tail sentinel */
    assert(result == &list->tail);
    /* The tail sentinel must be unchanged (frame condition) */
    assert(result == pre_tail);
    /* The list must remain valid after the call */
    assert(aws_linked_list_is_valid(list));
}
