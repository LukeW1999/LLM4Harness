#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* The function does not have a return value that indicates success or failure, so we assume it always succeeds. */
    assert(result == &list.tail);   // "Returns an iteration pointer for one past the last element in the list."

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.next == old.head.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
