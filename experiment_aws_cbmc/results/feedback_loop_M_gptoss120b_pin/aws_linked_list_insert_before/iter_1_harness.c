#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev; /* node before tail (may be &list.head) */

    /* 2. Allocate a node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    /* Ensure the node to add is not already part of the list */
    __CPROVER_assume(to_add != &list.head);
    __CPROVER_assume(to_add != &list.tail);
    __CPROVER_assume(to_add != old_last);

    /* 3. Call the function under test: insert before the tail sentinel */
    aws_linked_list_insert_before(&list.tail, to_add);

    /* 4. Post‑condition asserts */

    /* The inserted node links correctly */
    assert(to_add->next == &list.tail);
    assert(to_add->prev == old_last);

    /* The surrounding nodes now point to the inserted node */
    assert(list.tail.prev == to_add);
    assert(old_last->next == to_add);

    /* Sentinel invariants that never change */
    assert(list.head.prev == old.head.prev);   /* should remain NULL */
    assert(list.tail.next == old.tail.next);   /* should remain NULL */

    /* The inserted node is now considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 5. The list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));
}
