#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
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

    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    aws_linked_list_swap_contents(&a, &b);

    if (aws_linked_list_is_valid(&a) && aws_linked_list_is_valid(&b)) {
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    } else {
        assert(false); // swap_contents should not fail
    }

    assert(a.head.prev == &a.tail);
    assert(b.head.prev == &b.tail);

    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
