#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness(void) {
    /* Create a minimal linked list with head and tail sentinels */
    struct aws_linked_list list;
    
    /* Initialize head and tail sentinels to form a valid empty list */
    list.head.prev = NULL;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;
    list.tail.next = NULL;
    
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Use the tail sentinel as 'before' - insert before tail */
    struct aws_linked_list_node *before = &list.tail;
    
    /* The node to insert */
    struct aws_linked_list_node to_add;
    
    /* Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;
    
    /* Preconditions */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    
    /* Call the function under test */
    aws_linked_list_insert_before(before, &to_add);
    
    /* Postconditions: changed fields */
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(before->prev == &to_add);
    assert(old_before_prev->next == &to_add);
    
    /* Postconditions: unchanged fields */
    assert(before->next == old_before_next);
    
    /* Validity invariants: bidirectional linkage around inserted node */
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(before->prev == &to_add);
    assert(old_before_prev->next == &to_add);
    
    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
