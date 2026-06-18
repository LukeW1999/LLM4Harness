#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_end_harness() {
    /* Declare and set up a valid doubly-linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state to check immutability */
    struct aws_linked_list old = list;

    /* Call the function under verification */
    const struct aws_linked_list_node *end = aws_linked_list_end(&list);

    /* Postcondition: returns pointer to the tail sentinel */
    assert(end == &list.tail);

    /* Immutability: no fields of the list structure are modified */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The list validity is preserved */
    assert(aws_linked_list_is_valid(&list));
}
