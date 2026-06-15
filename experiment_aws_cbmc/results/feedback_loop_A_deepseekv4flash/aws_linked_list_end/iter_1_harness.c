#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness() {
    /* Non-deterministic linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state for immutability check */
    struct aws_linked_list old = list;

    /* Call the function */
    const struct aws_linked_list_node *end = aws_linked_list_end(&list);

    /* Assert return value */
    assert(end == &list.tail);
    assert(end != NULL);

    /* Assert list unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prevhed);

    /* Assert validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
