#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness() {
    /* nondeterministically allocate a linked list with up to MAX_LINKED_LIST_ITEM_ALLOCATION nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* save old state */
    struct aws_linked_list old_list = list;

    /* call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* assert that the list is unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* assert that the list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* assert that the returned pointer is indeed the back of the list */
    assert(result == list.tail.prev);
}
