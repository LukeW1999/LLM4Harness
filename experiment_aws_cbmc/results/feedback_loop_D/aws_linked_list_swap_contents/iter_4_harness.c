#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;
    struct aws_linked_list old_a;
    struct aws_linked_list old_b;

    ensure_allocated_aws_linked_list(&a);
    ensure_allocated_aws_linked_list(&b);
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    old_a = a;
    old_b = b;

    aws_linked_list_swap_contents(&a, &b);

    // Validity invariants
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    // Frame conditions for a
    if (!aws_linked_list_empty(&old_b)) {
        assert(a.head.next == old_b.head.next);
        assert(a.tail.prev == old_b.tail.prev);
    } else {
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    }

    assert(a.head.prev == &a.tail);
    assert(a.tail.next == &a.head);

    // Frame conditions for b
    if (!aws_linked_list_empty(&old_a)) {
        assert(b.head.next == old_a.head.next);
        assert(b.tail.prev == old_a.tail.prev);
    } else {
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    }

    assert(b.head.prev == &b.tail);
    assert(b.tail.next == &b.head);
}
