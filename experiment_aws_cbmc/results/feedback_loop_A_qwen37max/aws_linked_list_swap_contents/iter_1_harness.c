#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last = b.tail.prev;

    aws_linked_list_swap_contents(&a, &b);

    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    if (a_was_empty) {
        assert(aws_linked_list_empty(&b));
    } else {
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == old_a_first);
        assert(b.tail.prev == old_a_last);
    }

    if (b_was_empty) {
        assert(aws_linked_list_empty(&a));
    } else {
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == old_b_first);
        assert(a.tail.prev == old_b_last);
    }
}
