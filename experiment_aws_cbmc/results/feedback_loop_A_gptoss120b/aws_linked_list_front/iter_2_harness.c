#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_front_harness(void) {
    /* Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old = list;

    /* Call function under test */
    struct aws_linked_list_node *rval = aws_linked_list_front(&list);

    /* Postconditions */
    if (old.head.next == &old.tail) {
        /* Empty list: front should return NULL */
        assert(rval == NULL);
    } else {
        /* Non‑empty list: front should return the first element */
        assert(rval == old.head.next);
    }

    /* Unchanged fields */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
