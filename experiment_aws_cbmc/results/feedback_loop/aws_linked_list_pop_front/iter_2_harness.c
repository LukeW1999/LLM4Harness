#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_pop_front_harness() {
    /* data structure */
    struct aws_linked_list list;

    /* Ensure the list is allocated and initialized */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);

    /* Add at least one node to the list */
    struct aws_linked_list_node node;
    aws_linked_list_push_front(&list, &node);

    /* Assume the preconditions. The function requires that list != NULL and list is not empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Keep the old next node of the head */
    struct aws_linked_list_node *old_next_first = list.head.next->next;

    /* perform operation under verification */
    struct aws_linked_list_node *ret = aws_linked_list_pop_front(&list);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(ret == &node);
    assert(ret->next == NULL && ret->prev == NULL);
    assert(list.head.next == old_next_first);
}
