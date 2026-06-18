#include <assert.h>
#include <stddef.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness() {
    /* 1. Declare and bound data structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    bool old_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *begin = aws_linked_list_begin(&list);

    /* 4. Assert postconditions — no failure path for this accessor */

    /* Returns the first element (or tail sentinel if empty), which must match original head.next */
    assert(begin == old_head_next);

    /* The list must remain completely unchanged */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);
    assert(aws_linked_list_empty(&list) == old_empty);

    /* 5. Validity invariant must hold */
    assert(aws_linked_list_is_valid(&list));
}
