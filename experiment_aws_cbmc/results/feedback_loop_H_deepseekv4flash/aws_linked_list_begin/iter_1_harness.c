#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness() {
    /* nondeterministic list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* save original state */
    struct aws_linked_list old = list;

    /* call function under test */
    struct aws_linked_list_node *rval = aws_linked_list_begin(&list);

    /* assert return value */
    assert(rval == list.head.next);

    /* assert no changes to list */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* assert validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
