#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_rbegin_harness(void) {
    /* 1. Declare and allocate the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    /* 4. Assert that no fields of the list changed */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 5. Assert post‑condition about the returned pointer */
    if (aws_linked_list_empty(&list)) {
        /* When the list is empty, rbegin should point to the head sentinel */
        assert(r == &list.head);
    } else {
        /* When the list is non‑empty, rbegin should point to the last real node */
        assert(r != &list.head);
        assert(r->next == &list.tail);
        /* The returned node must be properly linked in both directions */
        assert(r->prev->next == r);
        assert(r->next->prev == r);
    }

    /* 6. Assert validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
