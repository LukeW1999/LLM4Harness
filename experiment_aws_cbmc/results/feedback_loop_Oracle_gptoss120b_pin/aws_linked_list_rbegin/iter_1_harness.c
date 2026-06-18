#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;

    /* Precondition: the list must be a valid linked list */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Snapshot of the list structure before the call */
    struct aws_linked_list old = list;

    /* Call the function under test */
    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    /* Postcondition 1: return value matches the list's tail.prev */
    assert(r == old.tail.prev);

    /* Postcondition 2: the list structure is unchanged (frame condition) */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Postcondition 3: behavior depending on emptiness of the list */
    if (aws_linked_list_empty(&list)) {
        /* For an empty list, rbegin should point to the head sentinel */
        assert(r == &list.head);
    } else {
        /* For a non‑empty list, r should be the last user node */
        assert(r != &list.head);
        /* The last node's next must be the tail sentinel */
        assert(r->next == &list.tail);
        /* The bidirectional connections of the last node must be valid */
        assert(aws_linked_list_node_next_is_valid(r));
        assert(aws_linked_list_node_prev_is_valid(r));
    }

    return 0;
}
