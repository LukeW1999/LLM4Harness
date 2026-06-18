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
    /* We'll use a node that is already in the list - pick a nondet node */
    /* For simplicity, use the tail sentinel as 'before' (insert before tail = push_back) */
    /* Or use head.next if list is non-empty */
    
    /* Create to_add node */
    struct aws_linked_list_node to_add;
    
    /* Choose 'before' to be a node already in the list.
       We'll use the tail sentinel node as 'before', which is always valid. */
    struct aws_linked_list_node *before = &list.tail;
    
    /* Save state before the call */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;
    
    /* Precondition: before must be a valid node with valid prev pointer */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* Postconditions from the implementation:
       to_add->next = before
       to_add->prev = before->prev (old value)
       before->prev->next (old before->prev) = to_add
       before->prev = to_add
    */
    
    /* 1. Changed fields: to_add's next and prev, before's prev, old_before_prev's next */
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(before->prev == &to_add);
    assert(old_before_prev->next == &to_add);
    
    /* 2. Unchanged fields: before's next pointer should not change */
    assert(before->next == old_before_next);
    
    /* 3. Bidirectional linkage: to_add is properly linked */
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    
    /* 4. Validity invariant: list remains valid after insertion */
    assert(aws_linked_list_is_valid(&list));
    
    /* 5. List is non-empty after insertion */
    assert(!aws_linked_list_empty(&list));
}
