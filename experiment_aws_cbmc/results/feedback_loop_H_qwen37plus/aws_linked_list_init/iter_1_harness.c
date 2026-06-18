#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(&a != &b);

    struct aws_linked_list_node *a_first = a.head.next;
    struct aws_linked_list_node *a_last = a.tail.prev;
    struct aws_linked_list_node *b_first = b.head.next;
    struct aws_linked_list_node *b_last = b.tail.prev;

    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    aws_linked_list_swap_contents(&a, &b);

    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    if (b_was_empty) {
        assert(aws_linked_list_empty(&a));
    } else {
        assert(a.head.next == b_first);
        assert(a.tail.prev == b_last);
    }

    if (a_was_empty) {
        assert(aws_linked_list_empty(&b));
    } else {
        assert(b.head.next == a_first);
        assert(b.tail.prev == a_last);
    }
}
