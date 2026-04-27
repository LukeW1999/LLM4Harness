// === STEP 1: SUCCESS PATH ===
// aws_linked_list_insert_after always succeeds (void return).
// After the call:
//   - to_add->prev: CHANGES to after
//   - to_add->next: CHANGES to old after->next
//   - after->next->prev (the node that was after->next before): CHANGES to to_add
//   - after->next: CHANGES to to_add
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always executes.
//
// === STEP 3: FRAME CONDITIONS ===
// after (struct aws_linked_list_node *):
//   - after->prev: UNCHANGED
//   - after->next: CHANGED to to_add
// to_add (struct aws_linked_list_node *):
//   - to_add->prev: CHANGED to after
//   - to_add->next: CHANGED to old after->next
// old_after_next (the node that was after->next before the call):
//   - old_after_next->prev: CHANGED to to_add
//   - old_after_next->next: UNCHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
// The doubly-linked structure around the insertion point must be consistent:
//   - to_add->prev == after
//   - to_add->next == old_after_next
//   - after->next == to_add
//   - old_after_next->prev == to_add

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_insert_after_harness(void) {
    /* Allocate two nodes: 'after' and 'to_add' */
    struct aws_linked_list_node *after = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *after_next = malloc(sizeof(struct aws_linked_list_node));

    /* Ensure all pointers are non-null */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(to_add != NULL);
    __CPROVER_assume(after_next != NULL);

    /* Set up a minimal valid linked list segment:
       some_prev <-> after <-> after_next
       We need after->next to point to a valid node with a prev pointer */
    after->next = after_next;
    after_next->prev = after;

    /* after->prev can be anything valid (nondet), but must not be NULL for a real list */
    struct aws_linked_list_node *after_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_prev != NULL);
    after->prev = after_prev;
    after_prev->next = after;

    /* after_next->next can be anything */
    struct aws_linked_list_node *after_next_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_next_next != NULL);
    after_next->next = after_next_next;

    /* to_add can have arbitrary initial state */
    /* (its prev and next will be overwritten) */

    /* Save old state */
    struct aws_linked_list_node *old_after_next = after->next; /* == after_next */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next_next = after_next->next;

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* === STEP 1 assertions: verify the changes === */

    /* to_add->prev must be after */
    assert(to_add->prev == after);

    /* to_add->next must be old after->next (i.e., after_next) */
    assert(to_add->next == old_after_next);

    /* after->next must now be to_add */
    assert(after->next == to_add);

    /* old_after_next->prev must now be to_add */
    assert(old_after_next->prev == to_add);

    /* === STEP 3 frame condition assertions: verify unchanged fields === */

    /* after->prev must be unchanged */
    assert(after->prev == old_after_prev);

    /* after_next->next must be unchanged */
    assert(after_next->next == old_after_next_next);

    /* === STEP 4: structural validity of the insertion point === */

    /* The chain: after <-> to_add <-> after_next must be consistent */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == after_next);
    assert(after_next->prev == to_add);
}
