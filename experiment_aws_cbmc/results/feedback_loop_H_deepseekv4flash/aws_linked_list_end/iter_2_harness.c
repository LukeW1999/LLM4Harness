#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness() {
    /* Initialize an empty valid linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Precondition: list is valid */
    assert(aws_linked_list_is_valid(&list));

    /* Save the old list state to verify immutability */
    struct aws_linked_list old = list;

    /* Call the function under test */
    const struct aws_linked_list_node *rval = aws_linked_list_end(&list);

    /* Postcondition: return value is the address of the tail node */
    assert(rval == &list.tail);

    /* The list must remain unchanged: all pointer fields identical */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Validity invariants still hold after the call */
    assert(aws_linked_list_is_valid(&list));
}
