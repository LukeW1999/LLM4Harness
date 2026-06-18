#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Set up the 'after' node and 'to_add' node */
    struct aws_linked_list_node *after = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after != NULL);

    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* 'after' must have a valid next pointer (non-NULL) for the function to work */
    struct aws_linked_list_node *after_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_next != NULL);

    /* Set up the bidirectional link: after <-> after_next */
    after->next = after_next;
    after_next->prev = after;

    /* after->prev can be anything (not used by insert_after) */
    /* after_next->next can be anything (not used by insert_after) */

    /* to_add's initial state doesn't matter for the operation */

    /* Save old state */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next = after->next;  /* == after_next */
    struct aws_linked_list_node *old_after_next_next = after_next->next;

    /* 2. Call function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 3. Assert postconditions */

    /* Changed fields: to_add is inserted immediately after 'after' */
    /* to_add->prev should point to after */
    assert(to_add->prev == after);

    /* to_add->next should point to the old after->next (after_next) */
    assert(to_add->next == after_next);

    /* after->next should now point to to_add */
    assert(after->next == to_add);

    /* after_next->prev should now point to to_add */
    assert(after_next->prev == to_add);

    /* Unchanged fields: after->prev should not have changed */
    assert(after->prev == old_after_prev);

    /* after_next->next should not have changed */
    assert(after_next->next == old_after_next_next);

    /* Bidirectional linkage: after <-> to_add <-> after_next */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == after_next);
    assert(after_next->prev == to_add);

    /* Node next_is_valid checks: prev of next points back to node */
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(after_next));
}
