#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness() {
    /* Initialize and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state for immutability check */
    struct aws_linked_list old_list = list;

    /* Call the function */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* Check the return value matches tail.prev */
    assert(result == list.tail.prev);

    /* Check that the list structure is unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* Check validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
