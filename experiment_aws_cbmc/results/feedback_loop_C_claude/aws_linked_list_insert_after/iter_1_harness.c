// === STEP 1: SUCCESS PATH ===
// aws_linked_list_insert_after is void, no return value.
// After the call:
//   - to_add->prev: CHANGES to after
//   - to_add->next: CHANGES to old after->next
//   - after->next->prev (old after->next): CHANGES to to_add
//   - after->next: CHANGES to to_add
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always succeeds.
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
// After insertion:
//   - to_add->prev == after
//   - to_add->next == old_after_next
//   - after->next == to_add
//   - old_after_next->prev == to_add
//   - The bidirectional links are consistent

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_insert_after_harness(void) {
    /* Allocate a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, 2);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need an 'after' node that is in the list.
       We'll use the head node as 'after' to keep it simple and valid. */
    struct aws_linked_list_node *after = list.head.next;
    /* after must not be the tail (we need after->next to be valid) */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL);

    /* Allocate a new node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_next_next = after->next->next;

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* === Verify frame conditions and postconditions === */

    /* to_add->prev should be after */
    assert(to_add->prev == after);

    /* to_add->next should be old after->next */
    assert(to_add->next == old_after_next);

    /* after->next should now be to_add */
    assert(after->next == to_add);

    /* old_after_next->prev should now be to_add */
    assert(old_after_next->prev == to_add);

    /* after->prev should be unchanged */
    assert(after->prev == old_after_prev);

    /* old_after_next->next should be unchanged */
    assert(old_after_next->next == old_after_next_next);

    /* Bidirectional consistency checks */
    assert(after->next->prev == after);
    assert(to_add->next->prev == to_add);
    assert(to_add->prev->next == to_add);
}
