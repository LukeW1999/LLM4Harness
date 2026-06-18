#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. We need a 'before' node that is already in the list.
       Use the tail sentinel as 'before' to insert at the back,
       or use head.next if the list is non-empty.
       For generality, use the tail sentinel (always valid). */
    struct aws_linked_list_node *before = &list.tail;

    /* 3. Create the node to insert */
    struct aws_linked_list_node to_add_node;
    struct aws_linked_list_node *to_add = &to_add_node;

    /* 4. Save old state before the call */
    /* Save the node that was previously before 'before' */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* 5. Preconditions:
       - 'before' must be a valid node in the list (its prev->next == before) */
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(before->prev->next == before);

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Assert postconditions */

    /* to_add->next should point to before */
    assert(to_add->next == before);

    /* to_add->prev should point to old before->prev */
    assert(to_add->prev == old_before_prev);

    /* before->prev should now point to to_add */
    assert(before->prev == to_add);

    /* old_before_prev->next should now point to to_add */
    assert(old_before_prev->next == to_add);

    /* The bidirectional linkage between to_add and before */
    assert(to_add->next == before);
    assert(before->prev == to_add);

    /* The bidirectional linkage between old_before_prev and to_add */
    assert(old_before_prev->next == to_add);
    assert(to_add->prev == old_before_prev);

    /* The list should still be valid after insertion */
    assert(aws_linked_list_is_valid(&list));

    /* The list should not be empty after insertion */
    assert(!aws_linked_list_empty(&list));
}
