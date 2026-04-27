#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Add at least one node to the list */
    struct aws_linked_list_node node;
    __CPROVER_assume(&node != NULL);
    aws_linked_list_push_back(&list, &node);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_pop_back(&list);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path (always succeeds for non-empty list) */
    if (old_last->prev != &list.head) {
        assert(list.tail.prev == old_last->prev); // tail now points to previous node
        assert(old_last->prev->next == &list.tail); // previous node links forward to tail
    } else {
        assert(list.tail.prev == &list.head); // tail prev points to head if list becomes empty
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.prev == NULL); // head.prev remains unchanged
    assert(list.tail.next == NULL); // tail.next remains unchanged

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
