#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    /* No state is modified by this function, but we keep the list valid */

    /* 3. Call function under test */
    /* We use the head node as a valid, non-NULL node to test the function */
    struct aws_linked_list_node *node = &list.head;
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    /* The function simply returns the next pointer of the given node */
    assert(result == node->next);

    /* 5. Assert fields that must NOT change regardless of result */
    /* This function is a pure getter and does not modify any state */

    /* 6. Assert validity invariants always hold */
    assert(aws_linked_list_is_valid(&list));
}
