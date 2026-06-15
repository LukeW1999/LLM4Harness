#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_linked_list_insert_after_harness() {
    /* Data structures */
    struct aws_linked_list list;
    struct aws_linked_list_node *after = NULL;
    struct aws_linked_list_node to_add;

    /* Ensure the list is allocated and has at least one node */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Pick a non-sentinel node as 'after' */
    after = list.head.next;
    __CPROVER_assume(after != &list.tail);
    __CPROVER_assume(after != NULL);

    /* Ensure to_add is a fresh node (not in any list) */
    to_add.next = NULL;
    to_add.prev = NULL;

    /* Save the old state of nodes that will be modified by the function */
    struct aws_linked_list_node *old_next = after->next;
    struct aws_linked_list_node *old_prev = after->prev; // not directly modified but kept for completeness

    /* Call the function */
    aws_linked_list_insert_after(after, &to_add);

    /* Postconditions */
    // 1. The list is still valid
    assert(aws_linked_list_is_valid_deep(&list));

    // 2. to_add is correctly linked after 'after'
    assert(after->next == &to_add);
    assert(to_add.prev == after);
    assert(to_add.next == old_next);
    assert(old_next->prev == &to_add);

    // 3. Nodes before 'after' are unaffected (prev pointer of after unchanged)
    assert(after->prev == old_prev);

    // 4. The node 'to_add' is now in the list, so it's not NULL
    assert(to_add.next != NULL);
    assert(to_add.prev != NULL);

    // 5. The list is not empty (still has at least one node)
    assert(!aws_linked_list_empty(&list));
}
