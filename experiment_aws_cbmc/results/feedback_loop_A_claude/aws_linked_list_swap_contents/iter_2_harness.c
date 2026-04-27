#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness(void) {
    /* 1. Declare and initialize two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Initialize lists to valid empty state */
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* 2. Assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 3. Save state before the call */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    struct aws_linked_list_node *a_first = a.head.next;
    struct aws_linked_list_node *a_last  = a.tail.prev;
    struct aws_linked_list_node *b_first = b.head.next;
    struct aws_linked_list_node *b_last  = b.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 5. Assert postconditions */

    /* Both lists must be valid after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Check what A contains after swap (should be B's old contents) */
    if (b_was_empty) {
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == b_first);
        assert(a.tail.prev == b_last);
        assert(b_first->prev == &a.head);
        assert(b_last->next == &a.tail);
    }

    /* Check what B contains after swap (should be A's old contents) */
    if (a_was_empty) {
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == a_first);
        assert(b.tail.prev == a_last);
        assert(a_first->prev == &b.head);
        assert(a_last->next == &b.tail);
    }
}
