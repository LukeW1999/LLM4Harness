#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_rbegin_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_linked_list_node old_head = list.head;
    struct aws_linked_list_node old_tail = list.tail;

    /* 3. Call function under test */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* 4. Assert postconditions */
    /* The function returns a pointer to the last element (tail.prev) */
    assert(rval == list.tail.prev);

    /* 5. Assert fields that must NOT change */
    assert(list.head.next == old_head.next);
    assert(list.head.prev == old_head.prev);
    assert(list.tail.next == old_tail.next);
    assert(list.tail.prev == old_tail.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
