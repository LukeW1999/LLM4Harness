#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    struct aws_linked_list old_list = list; // Save old state for immutability checks

    /* 2. Call function under test */
    aws_linked_list_init(&list);

    /* 3. Assert postconditions for BOTH success and failure paths */
    /* Since aws_linked_list_init does not return a value, we only have success path */
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);

    /* 4. Assert fields that must NOT change regardless of result */
    // No additional fields to assert as the entire list is re-initialized

    /* 5. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
