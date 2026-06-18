#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* list must have at least one element */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_front = list.head.next;
    __CPROVER_assume(old_front != &list.tail);

    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* Check that the correct node was returned */
    assert(popped == old_front);

    /* Check that the returned node is properly isolated */
    assert(popped->next == NULL);
    assert(popped->prev == NULL);

    /* Check list invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
