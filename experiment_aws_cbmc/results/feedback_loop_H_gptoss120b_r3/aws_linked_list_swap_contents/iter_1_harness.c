#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness(void) {
    /* 1. Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last  = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last  = b.tail.prev;

    bool old_a_empty = (old_a_first == &old_a.tail);
    bool old_b_empty = (old_b_first == &old_b.tail);

    /* 3. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* 5. Post‑condition: contents are swapped */

    /* a now contains the former contents of b */
    if (old_b_empty) {
        /* b was empty → a must be empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a.head.next should point to the first node that used to belong to b */
        assert(a.head.next == old_b_first);
        /* a.tail.prev should point to the last node that used to belong to b */
        assert(a.tail.prev == old_b_last);
        /* bidirectional links for the new first/last nodes */
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
    }

    /* b now contains the former contents of a */
    if (old_a_empty) {
        /* a was empty → b must be empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(b.head.next == old_a_first);
        assert(b.tail.prev == old_a_last);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 6. Unchanged fields: the list structures themselves (head/tail node objects) are
       still the same objects; only their internal pointers may have been updated,
       which is already covered by the assertions above. No other fields exist. */
}
