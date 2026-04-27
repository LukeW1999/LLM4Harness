#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Precondition: list must not be empty (back() requires non-empty list) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state before calling */
    struct aws_linked_list_node *expected_back = list.tail.prev;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 5. Assert postconditions */

    /* Return value must equal tail.prev */
    assert(result == expected_back);

    /* Result must not be NULL */
    assert(result != NULL);

    /* The returned node must be the one before tail */
    assert(result->next == &list.tail);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
