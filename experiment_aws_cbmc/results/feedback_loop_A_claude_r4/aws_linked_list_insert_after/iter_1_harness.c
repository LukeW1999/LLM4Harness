#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Set up the linked list with non-deterministic length */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Pick a non-deterministic node already in the list to insert after.
       We use the head node as a simple valid "after" node (it's always present). */
    struct aws_linked_list_node *after = list.head.next;
    /* after must be a valid node (not the tail itself, but can be head.next which could be tail) */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL);
    __CPROVER_assume(after->prev != NULL);

    /* 3. Create the node to insert */
    struct aws_linked_list_node to_add_node;
    struct aws_linked_list_node *to_add = &to_add_node;

    /* 4. Save old state */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Assert postconditions */

    /* Changed fields: to_add is inserted immediately after 'after' */
    /* to_add->prev should point to after */
    assert(to_add->prev == after);

    /* to_add->next should point to what was after->next before the call */
    assert(to_add->next == old_after_next);

    /* after->next should now point to to_add */
    assert(after->next == to_add);

    /* The old next node's prev should now point to to_add */
    assert(old_after_next->prev == to_add);

    /* Unchanged fields: after->prev should not have changed */
    assert(after->prev == old_after_prev);

    /* Bidirectional linkage checks */
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_next_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(to_add));

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
