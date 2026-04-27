#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness() {
    /* data structure */
    struct aws_linked_list list;

    /* allocate and initialize the linked list */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* perform operation under verification */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* assertions */
    assert(result == &list.tail);
    assert(aws_linked_list_is_valid(&list));
}
