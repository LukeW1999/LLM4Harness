#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Set up the 'after' node with non-deterministic next/prev pointers */
    struct aws_linked_list_node after;
    struct aws_linked_list_node after_next;

    /* Wire up: after.next = &after_next, after_next.prev = &after */
    after.next = &after_next;
    after.prev = NULL; /* not used by insert_after */
    after_next.prev = &after;
    after_next.next = NULL; /* not used by insert_after */

    /* 2. Set up the node to insert */
    struct aws_linked_list_node to_add;
    /* to_add's prev/next will be overwritten by the function */

    /* 3. Save old state */
    struct aws_linked_list_node *old_after_next = after.next; /* = &after_next */

    /* 4. Call function under test */
    aws_linked_list_insert_after(&after, &to_add);

    /* 5. Assert postconditions */

    /* to_add.prev should point to after */
    assert(to_add.prev == &after);

    /* to_add.next should point to what was after->next before the call */
    assert(to_add.next == old_after_next);

    /* after->next should now point to to_add */
    assert(after.next == &to_add);

    /* The old after_next.prev should now point to to_add */
    assert(after_next.prev == &to_add);

    /* Verify bidirectional linkage: after <-> to_add <-> after_next */
    assert(after.next == &to_add);
    assert(to_add.prev == &after);
    assert(to_add.next == &after_next);
    assert(after_next.prev == &to_add);

    /* Verify node_next_is_valid for after (after.next->prev == &after is now to_add.prev == &after) */
    assert(aws_linked_list_node_next_is_valid(&after));

    /* Verify node_prev_is_valid for after_next (after_next.prev->next == &after_next is now to_add.next == &after_next) */
    assert(aws_linked_list_node_prev_is_valid(&after_next));

    /* Verify node_next_is_valid for to_add */
    assert(aws_linked_list_node_next_is_valid(&to_add));

    /* Verify node_prev_is_valid for to_add */
    assert(aws_linked_list_node_prev_is_valid(&to_add));
}
