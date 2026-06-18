#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_end_harness() {
    /* Create and bound a valid linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save the state before the call */
    struct aws_linked_list old = list;

    /* Call the function under test */
    const struct aws_linked_list_node *rval = aws_linked_list_end(&list);

    /* Postcondition: the returned pointer must be the address of the tail sentinel */
    assert(rval == &list.tail);

    /* The list must not have been modified */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The list must still satisfy the validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
