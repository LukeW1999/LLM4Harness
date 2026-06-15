#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness() {
    /* data structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* ensure validity */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* save old state */
    struct aws_linked_list old = list;

    /* call function under verification */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* postcondition: returns pointer to first element */
    assert(result == list.head.next);

    /* immutability: list structure unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* invariant remains valid */
    assert(aws_linked_list_is_valid(&list));
}
