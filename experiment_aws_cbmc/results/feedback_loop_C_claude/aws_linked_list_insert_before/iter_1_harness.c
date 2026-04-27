// === STEP 1: SUCCESS PATH ===
// aws_linked_list_insert_before is void, always "succeeds" if preconditions met.
// After the call:
//   - to_add->next: CHANGES to before
//   - to_add->prev: CHANGES to old before->prev
//   - before->prev->next (old before->prev->next): CHANGES to to_add
//   - before->prev: CHANGES to to_add
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void with no error return.
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
// After insertion, the list should remain valid (bidirectional links intact).
// The inserted node should be properly linked between old before->prev and before.

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_insert_before_harness(void) {
    /* Allocate a linked list with some elements */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need a valid 'before' node that is in the list.
     * For simplicity, we use the tail sentinel as 'before',
     * which means we're inserting before the tail (i.e., at the back).
     * Alternatively, we can use any node in the list.
     * We'll use the tail to keep it simple and valid. */
    struct aws_linked_list_node *before = &list.tail;

    /* Allocate a new node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;
    struct aws_linked_list_node *old_before_prev_prev = before->prev->prev;

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* === STEP 1 assertions: verify the new links === */
    /* to_add->next should point to before */
    assert(to_add->next == before);
    /* to_add->prev should point to old before->prev */
    assert(to_add->prev == old_before_prev);
    /* old before->prev->next should now point to to_add */
    assert(old_before_prev->next == to_add);
    /* before->prev should now point to to_add */
    assert(before->prev == to_add);

    /* === STEP 3 frame condition assertions === */
    /* before->next should be unchanged */
    assert(before->next == old_before_next);
    /* old_before_prev->prev should be unchanged */
    assert(old_before_prev->prev == old_before_prev_prev);

    /* === STEP 4 validity invariants === */
    /* The list should still be valid after insertion */
    assert(aws_linked_list_is_valid(&list));

    /* Verify bidirectional linking of the new node */
    assert(to_add->next->prev == to_add);
    assert(to_add->prev->next == to_add);
}
