#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* save old state */
    struct aws_linked_list old_list = list;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(result == list.head.next);

    /* unchanged fields */
    assert(list.tail.prev == old_list.tail.prev);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
}
