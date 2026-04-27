#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rend_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* perform operation under verification */
    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(result == &list.head);
}
