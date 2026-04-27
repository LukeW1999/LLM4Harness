#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness() {
    /* data structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Keep the old first node of the linked list */
    struct aws_linked_list_node *old_front = list.head.next;

    /* Assume the list is not empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Assume the list is valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(result == old_front);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    if (!aws_linked_list_empty(&list)) {
        assert(list.head.next == old_front->next);
        assert(old_front->next->prev == &list.head);
    } else {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    }
}
