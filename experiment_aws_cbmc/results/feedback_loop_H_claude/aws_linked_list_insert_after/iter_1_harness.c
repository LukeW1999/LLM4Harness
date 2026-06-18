#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Declare and set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare the node to insert after (must be a valid node in the list) */
    struct aws_linked_list_node *after;
    __CPROVER_assume(after != NULL);
    /* after must have a valid next pointer */
    __CPROVER_assume(after->next != NULL);
    /* The connection must be bidirectional: after->next->prev == after */
    __CPROVER_assume(after->next->prev == after);

    /* 3. Declare the node to add */
    struct aws_linked_list_node *to_add;
    __CPROVER_assume(to_add != NULL);
    /* to_add must be different from after and after->next */
    __CPROVER_assume(to_add != after);
    __CPROVER_assume(to_add != after->next);

    /* 4. Save old state before calling */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* 5. Call function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Assert postconditions */

    /* to_add->prev should point to after */
    assert(to_add->prev == after);

    /* to_add->next should point to what was after->next before the call */
    assert(to_add->next == old_after_next);

    /* after->next should now point to to_add */
    assert(after->next == to_add);

    /* old_after_next->prev should now point to to_add */
    assert(old_after_next->prev == to_add);

    /* after->prev should be unchanged */
    assert(after->prev == old_after_prev);

    /* Bidirectional linkage: after <-> to_add */
    assert(after->next == to_add);
    assert(to_add->prev == after);

    /* Bidirectional linkage: to_add <-> old_after_next */
    assert(to_add->next == old_after_next);
    assert(old_after_next->prev == to_add);

    /* aws_linked_list_node_next_is_valid checks that node->next->prev == node */
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(to_add));
    assert(aws_linked_list_node_next_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(old_after_next));
}
