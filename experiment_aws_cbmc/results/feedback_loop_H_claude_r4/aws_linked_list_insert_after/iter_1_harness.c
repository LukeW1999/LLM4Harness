#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Set up a linked list with at least one node (the 'after' node) */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. We need an 'after' node that is already in the list.
       Use the head node as 'after' (head.next is valid since list is valid).
       Actually, we need a node that has a valid next pointer.
       The head node always has a valid next pointer (either tail or another node). */
    struct aws_linked_list_node *after = &list.head;

    /* 3. Create the node to insert */
    struct aws_linked_list_node to_add;

    /* 4. Save old state */
    struct aws_linked_list_node *old_after_next = after->next;

    /* Preconditions: after must have a valid next pointer */
    __CPROVER_assume(after->next != NULL);
    __CPROVER_assume(after->next->prev == after);

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, &to_add);

    /* 6. Assert postconditions */

    /* to_add.prev should point to after */
    assert(to_add.prev == after);

    /* to_add.next should point to what was after->next before the call */
    assert(to_add.next == old_after_next);

    /* after->next should now point to to_add */
    assert(after->next == &to_add);

    /* old_after_next->prev should now point to to_add */
    assert(old_after_next->prev == &to_add);

    /* The bidirectional linkage between after and to_add */
    assert(aws_linked_list_node_next_is_valid(after));

    /* The bidirectional linkage between to_add and old_after_next
       (only valid if old_after_next is not the tail, i.e., it has a valid next) */
    /* We check the prev direction: to_add.next->prev == &to_add */
    assert(to_add.next->prev == &to_add);

    /* after->prev should be unchanged */
    /* (we don't track old after->prev explicitly, but we can check list validity) */

    /* The list should still be valid after insertion */
    assert(aws_linked_list_is_valid(&list));
}
