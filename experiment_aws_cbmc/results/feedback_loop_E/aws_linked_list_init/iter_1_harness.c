#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_init_harness() {
    /* data structure */
    struct aws_linked_list list;

    /* allocation and bounding */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* perform operation under verification */
    aws_linked_list_init(&list);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
}
