#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Declare and set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, 2);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare the node to insert after (must be a valid node in the list or head) */
    struct aws_linked_list_node *after;
    struct aws_linked_list_node *to_add;

    /* Use head as the 'after' node to keep things simple and valid */
    /* after must have a valid next pointer */
    after = &list.head;

    /* to_add is a fresh node */
    to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* 3. Save old state before calling */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* 4. Call function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 5. Assert postconditions */

    /* to_add->prev should point to after */
    assert(to_add->prev == after);

    /* to_add->next should point to what after->next was before */
    assert(to_add->next == old_after_next);

    /* after->next should now point to to_add */
    assert(after->next == to_add);

    /* old_after_next->prev should now point to to_add */
    assert(old_after_next->prev == to_add);

    /* after->prev should be unchanged */
    assert(after->prev == old_after_prev);

    /* 6. Assert validity invariant */
    assert(aws_linked_list_is_valid(&list));

    /* The list should not be empty after insertion */
    assert(!aws_linked_list_empty(&list));

    /* Node linkage: to_add is properly doubly linked */
    assert(aws_linked_list_node_next_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(to_add));
}
