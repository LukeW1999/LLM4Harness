Looking at the function `aws_linked_list_insert_after`, let me analyze:

1. **What changes**: `to_add` is inserted immediately after `after`. So:
   - `to_add->prev = after`
   - `to_add->next = after->next` (the old next of after)
   - `after->next->prev = to_add` (old next now points back to to_add)
   - `after->next = to_add`

2. **What's unchanged**: The rest of the list structure, `after->prev`, the old `after->next->next`, etc.

3. **Failure**: This function has no return value (void), so no failure path.

4. **Validity invariants**: The nodes should maintain valid bidirectional linkage after insertion.

The function requires:
- `after` is a valid node with a valid `next` pointer
- `to_add` is a valid node
- `after->next` must be non-null (since we dereference it)

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need a node that is already in the list to insert after.
     * Use the head node as 'after' - it always has a valid next pointer */
    struct aws_linked_list_node *after = &list.head;

    /* Save the old next of 'after' before insertion */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* Create the node to insert */
    struct aws_linked_list_node to_add_node;
    struct aws_linked_list_node *to_add = &to_add_node;

    /* Preconditions: after must have a valid next pointer */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL);
    __CPROVER_assume(to_add != NULL);

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postconditions: verify the linkage is correct */

    /* to_add->prev should point to after */
    assert(to_add->prev == after);

    /* to_add->next should point to what was after->next before the call */
    assert(to_add->next == old_after_next);

    /* after->next should now point to to_add */
    assert(after->next == to_add);

    /* The old next node's prev should now point to to_add */
    assert(old_after_next->prev == to_add);

    /* after->prev should be unchanged */
    assert(after->prev == old_after_prev);

    /* Verify bidirectional linkage of the new node */
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_next_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(old_after_next));

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
