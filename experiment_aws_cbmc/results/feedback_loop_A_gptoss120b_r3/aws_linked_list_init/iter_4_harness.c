#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

/* Harness for aws_linked_list_pop_back */
void aws_linked_list_pop_back_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 2. Assume the list is valid and non-empty (preconditions) */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state needed for post-condition checks */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;          /* node that will be removed */
    struct aws_linked_list_node *old_prev = old_last->prev;          /* node that will become new tail */

    /* 4. Call the function under test */
    struct aws_linked_list_node *removed = aws_linked_list_pop_back(&list);

    /* 5. Post-condition assertions */

    /* The returned node is the former last node */
    assert(removed == old_last);

    /* The removed node has been reset (next and prev are NULL) */
    assert(removed->next == NULL);
    assert(removed->prev == NULL);

    /* The list tail now points to the previous node (or head if list became empty) */
    if (old_prev == &list.head) {
        /* List is now empty */
        assert(list.tail.prev == &list.head);
        assert(list.head.next == &list.tail);
    } else {
        assert(list.tail.prev == old_prev);
    }

    /* head.next is unchanged by pop_back */
    assert(list.head.next == old.head.next);

    /* Invariant fields of the list structure remain unchanged */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);

    /* The list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* The removed node is no longer considered to be in a list */
    assert(!aws_linked_list_node_is_in_list(removed));
}
