#include <assert.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;

    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    if (list.head.next == &list.tail) {
        /* Empty list */
        assert(r == NULL);
    } else {
        /* Non‑empty list */
        assert(r == list.tail.prev);
    }

    /* List fields must be unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
