#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;

    struct aws_linked_list_node *front = aws_linked_list_front(&list);

    /* The returned pointer must equal head->next (the front element) */
    assert(front == list.head.next);

    /* The list itself must remain completely unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The list must still be valid after the call */
    assert(aws_linked_list_is_valid(&list));
}
