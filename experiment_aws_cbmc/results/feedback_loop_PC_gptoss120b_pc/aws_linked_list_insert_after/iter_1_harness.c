#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure the list is non‑empty so that we have a valid “after” node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Choose an existing node from the list as the “after” node */
    struct aws_linked_list_node *after = list.head.next;
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL); /* after must not be the tail sentinel */

    /* 3. Allocate a fresh node to add */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    /* The node to be added must not already be linked */
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL);
    __CPROVER_assume(to_add->prev == NULL);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 4. Snapshot state that will be examined after the call */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_next_next = old_after_next->next;
    struct aws_linked_list_node *old_next_prev = old_after_next->prev; /* should be after */

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Post‑conditions: changed fields */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(old_after_next->prev == to_add);

    /* 7. Unchanged fields (for the nodes we tracked) */
    assert(after->prev == old_after_prev);
    assert(old_after_next->next == old_next_next);
    assert(old_next_prev == after); /* old_next_prev was after before insertion */

    /* 8. The newly added node must now be reported as being in a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 9. Global invariants: the list remains valid */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
