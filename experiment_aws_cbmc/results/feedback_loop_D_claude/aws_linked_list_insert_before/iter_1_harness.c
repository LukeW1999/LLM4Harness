// === STEP 1: SUCCESS PATH ===
// aws_linked_list_insert_before always succeeds (void return).
// After the call:
//   - to_add->next: CHANGES to before
//   - to_add->prev: CHANGES to old before->prev
//   - before->prev->next (old before->prev->next): CHANGES to to_add
//   - before->prev: CHANGES to to_add
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always executes.
//
// === STEP 3: FRAME CONDITIONS ===
// before (struct aws_linked_list_node *):
//   - before->next: UNCHANGED
//   - before->prev: CHANGED to to_add
// to_add (struct aws_linked_list_node *):
//   - to_add->next: CHANGED to before
//   - to_add->prev: CHANGED to old before->prev
// old_before_prev (the node that was before->prev):
//   - old_before_prev->next: CHANGED to to_add
//   - old_before_prev->prev: UNCHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
// After insertion:
//   - to_add->next == before
//   - to_add->prev == old_before_prev
//   - old_before_prev->next == to_add
//   - before->prev == to_add
//   - The doubly-linked structure is consistent around to_add

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_insert_before_harness(void) {
    /* Allocate a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, 4);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need a 'before' node that is already in the list.
       We'll use a node that is reachable - for simplicity, use head.next
       if the list is non-empty, or tail itself. */
    /* Pick 'before' as a node in the list - use head.next (could be tail) */
    struct aws_linked_list_node *before = list.head.next;
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);

    /* Allocate a fresh node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node old_before = *before;
    struct aws_linked_list_node old_before_prev_node = *old_before_prev;

    /* Call the function */
    aws_linked_list_insert_before(before, to_add);

    /* === STEP 1 assertions: verify the changes === */
    /* to_add->next points to before */
    assert(to_add->next == before);
    /* to_add->prev points to old before->prev */
    assert(to_add->prev == old_before_prev);
    /* old before->prev->next now points to to_add */
    assert(old_before_prev->next == to_add);
    /* before->prev now points to to_add */
    assert(before->prev == to_add);

    /* === STEP 3 frame condition assertions === */
    /* before->next is unchanged */
    assert(before->next == old_before.next);
    /* old_before_prev->prev is unchanged */
    assert(old_before_prev->prev == old_before_prev_node.prev);

    /* === STEP 4 validity invariants === */
    /* The doubly-linked structure is consistent around to_add */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(to_add->prev->next == to_add);
    assert(to_add->next->prev == to_add);
}
