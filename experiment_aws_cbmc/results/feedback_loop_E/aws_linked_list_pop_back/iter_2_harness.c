#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    /* data structure */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume the list is not empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Assume the list is valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Keep the old last node of the linked list */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* Keep the old second last node of the linked list */
    struct aws_linked_list_node *old_second_last = old_last->prev;

    /* Save the old state of the list */
    struct aws_linked_list old_list = list;

    /* perform operation under verification */
    struct aws_linked_list_node *popped_node = aws_linked_list_pop_back(&list);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(popped_node == old_last);
    assert(popped_node->next == NULL && popped_node->prev == NULL);
    assert(list.tail.prev == old_second_last);
    if (old_second_last != &list.head) {
        assert(old_second_last->next == &list.tail);
    }

    /* Check that the rest of the list remains unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == NULL);
}
