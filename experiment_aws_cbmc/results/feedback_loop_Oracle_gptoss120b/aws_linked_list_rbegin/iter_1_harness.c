#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;

    /* nondeterministically allocate and initialize the list structure */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy of the list structure to check for unintended modifications */
    struct aws_linked_list list_old = list;

    /* Call the function under verification */
    struct aws_linked_list_node *rbegin = aws_linked_list_rbegin(&list);

    /* -------------------------------------------------------------
       Post‑condition checks
       ------------------------------------------------------------- */

    /* 1. Return value correctness */
    assert(rbegin == list.tail.prev);

    if (aws_linked_list_empty(&list)) {
        /* For an empty list the tail's prev points to the head sentinel */
        assert(rbegin == &list.head);
    } else {
        /* For a non‑empty list the returned node must not be the head sentinel */
        assert(rbegin != &list.head);
        /* The returned node must have a valid backward link */
        assert(aws_linked_list_node_prev_is_valid(rbegin));
    }

    /* 2. No modification of the list structure (frame condition) */
    assert(list.head.next == list_old.head.next);
    assert(list.head.prev == list_old.head.prev);
    assert(list.tail.next == list_old.tail.next);
    assert(list.tail.prev == list_old.tail.prev);

    return 0;
}
