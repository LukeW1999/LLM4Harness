#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_linked_list_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save the old state of the list (head and tail sentinels) */
    struct aws_linked_list old = list;

    /* 2. Call function under test */
    struct aws_linked_list_node *back = aws_linked_list_back(&list);

    /* 3. Assert postconditions */

    /* The returned pointer must be the node immediately before the tail */
    assert(back == list.tail.prev);

    /* The list must remain completely unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The list invariant must still hold (function does not modify, but it's a good check) */
    assert(aws_linked_list_is_valid(&list));
}
