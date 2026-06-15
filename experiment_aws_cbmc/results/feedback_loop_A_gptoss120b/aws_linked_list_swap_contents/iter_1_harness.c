#include <assert.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_swap_contents_harness(void) {
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Allocate and bound the lists */
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume the lists are initially valid */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old states for comparison */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Validity invariants must hold after the call */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* Post‑conditions for list a (now contains old b) */
    if (aws_linked_list_empty(&old_b)) {
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* Post‑conditions for list b (now contains old a) */
    if (aws_linked_list_empty(&old_a)) {
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }

    /* Unchanged fields: the list structures themselves (head/tail objects) */
    assert(&a.head == &old_a.head);
    assert(&a.tail == &old_a.tail);
    assert(&b.head == &old_b.head);
    assert(&b.tail == &old_b.tail);
}
