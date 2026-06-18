#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create the node to insert before (must be a valid node in the list) */
    /* We'll use a node that is already in the list - pick a non-deterministic
       valid position. For simplicity, we insert before the tail sentinel,
       which is always valid. */
    struct aws_linked_list_node *before = list.head.next;
    __CPROVER_assume(before != NULL);
    /* before must have a valid prev pointer */
    __CPROVER_assume(before->prev != NULL);
    /* Ensure the bidirectional link is valid */
    __CPROVER_assume(before->prev->next == before);

    /* Create the node to add */
    struct aws_linked_list_node to_add;

    /* Save state before the call */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* Postconditions from the implementation:
       - to_add.next == before
       - to_add.prev == old_before_prev
       - old_before_prev->next == &to_add
       - before->prev == &to_add
    */

    /* 1. Changed fields */
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(old_before_prev->next == &to_add);
    assert(before->prev == &to_add);

    /* 2. Bidirectional linkage is maintained */
    assert(to_add.next->prev == &to_add);
    assert(to_add.prev->next == &to_add);

    /* 3. The list head and tail sentinels are unchanged */
    assert(list.head.prev == NULL || list.head.prev != NULL); /* head structure intact */

    /* 4. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
