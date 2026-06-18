#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Pick a node 'after' from the list (not the tail sentinel) */
    struct aws_linked_list_node *after = NULL;
    for (struct aws_linked_list_node *node = list.head.next;
         node != &list.tail;
         node = node->next) {
        if (nondet_bool()) {
            after = node;
            break;
        }
    }
    if (after == NULL) {
        /* Default to head sentinel (valid for insertion at front) */
        after = &list.head;
    }
    /* after must not be the tail sentinel */
    __CPROVER_assume(after != &list.tail);

    /* 3. Prepare the node to add (not currently in any list) */
    struct aws_linked_list_node to_add;
    to_add.next = NULL;
    to_add.prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    /* 4. Save old state */
    struct aws_linked_list_node *old_next = after->next;

    /* 5. Call function under test */
    aws_linked_list_insert_after(after, &to_add);

    /* 6. Assert postconditions */
    /* to_add is now linked after 'after' */
    assert(to_add.prev == after);
    assert(to_add.next == old_next);
    assert(after->next == &to_add);
    assert(old_next->prev == &to_add);

    /* to_add is now in a list */
    assert(aws_linked_list_node_is_in_list(&to_add));

    /* after is still in a list (it was, and still is) */
    assert(aws_linked_list_node_is_in_list(after));

    /* The whole list remains valid */
    assert(aws_linked_list_is_valid_deep(&list));
}
