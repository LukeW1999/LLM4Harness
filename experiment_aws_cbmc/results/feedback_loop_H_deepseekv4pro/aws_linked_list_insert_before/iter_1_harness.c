#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Pick a node 'before' from the list (could be head, tail, or any internal node) */
    struct aws_linked_list_node *before = &list.head;
    size_t steps = nondet_size_t();
    __CPROVER_assume(steps <= MAX_LINKED_LIST_ITEM_ALLOCATION + 2); /* allow reaching tail */
    for (size_t i = 0; i < steps; i++) {
        before = before->next;
    }
    /* before is now a valid node in the list (non-NULL, part of the doubly-linked structure) */

    /* Create a new node to insert, initially detached */
    struct aws_linked_list_node to_add;
    to_add.next = NULL;
    to_add.prev = NULL;
    /* Ensure it is not already in a list */
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    /* 2. Save old state BEFORE calling */
    struct aws_linked_list_node *old_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* 3. Call function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* 4. Assert postconditions */
    /* to_add is now linked immediately before 'before' */
    assert(to_add.next == before);
    assert(to_add.prev == old_prev);
    assert(old_prev->next == &to_add);
    assert(before->prev == &to_add);

    /* before->next remains unchanged */
    assert(before->next == old_before_next);

    /* to_add is now part of the list */
    assert(aws_linked_list_node_is_in_list(&to_add));

    /* 5. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
