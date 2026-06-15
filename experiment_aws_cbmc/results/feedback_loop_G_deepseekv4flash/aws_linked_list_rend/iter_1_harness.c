#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rend_harness() {
    /* Non-deterministic structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    /* Assume list is valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state (the whole struct is small, we can save it) */
    struct aws_linked_list old = list;

    /* Call function */
    const struct aws_linked_list_node *rval = aws_linked_list_rend(&list);

    /* Postconditions */
    /* 1. Return value is &list.head */
    assert(rval == &list.head);
    /* 2. List unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    /* 3. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
