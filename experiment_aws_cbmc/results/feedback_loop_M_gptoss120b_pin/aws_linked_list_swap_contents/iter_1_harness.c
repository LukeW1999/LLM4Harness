#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness(void) {
    /* Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    bool old_a_empty = aws_linked_list_empty(&old_a);
    bool old_b_empty = aws_linked_list_empty(&old_b);

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Post‑condition: contents are swapped */
    if (old_b_empty) {
        assert(aws_linked_list_empty(&a));
    } else {
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    if (old_a_empty) {
        assert(aws_linked_list_empty(&b));
    } else {
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }
}
