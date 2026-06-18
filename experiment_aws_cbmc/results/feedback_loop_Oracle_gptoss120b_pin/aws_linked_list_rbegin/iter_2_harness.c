#include <aws/common/linked_list.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;

    /* Precondition: the list must be a valid linked list */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Snapshot of the list structure before the call */
    struct aws_linked_list old = list;

    /* Call the function under test */
    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    /* Postcondition: return value matches the list's tail.prev */
    assert(r == old.tail.prev);

    /* Frame condition: the list structure is unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
}
