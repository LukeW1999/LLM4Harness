#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list old_list = list;

    struct aws_linked_list_node *rval = aws_linked_list_front(&list);

    /* Return value is the first node */
    assert(rval == list.head.next);

    /* List is not modified */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
}
