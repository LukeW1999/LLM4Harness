// === STEP 1: SUCCESS PATH ===
// aws_linked_list_swap_contents always returns void (no return value).
// After the call:
//   - a->head.next: CHANGES to old b->head.next (or &a->head if b was empty)
//   - a->tail.prev: CHANGES to old b->tail.prev (or &a->tail if b was empty)
//   - a->head.prev: UNCHANGED (always NULL / sentinel)
//   - a->tail.next: UNCHANGED (always NULL / sentinel)
//   - b->head.next: CHANGES to old a->head.next (or &b->head if a was empty)
//   - b->tail.prev: CHANGES to old a->tail.prev (or &b->tail if a was empty)
//   - b->head.prev: UNCHANGED
//   - b->tail.next: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always executes.
//
// === STEP 3: FRAME CONDITIONS ===
// a (struct aws_linked_list):
//   - head.next: CHANGED - becomes old b->head.next (or &a->head if b empty)
//   - head.prev: UNCHANGED (not touched by this function)
//   - tail.prev: CHANGED - becomes old b->tail.prev (or &a->tail if b empty)
//   - tail.next: UNCHANGED (not touched by this function)
//   - The first node's prev pointer: CHANGED to point to &a->head
//   - The last node's next pointer: CHANGED to point to &a->tail
// b (struct aws_linked_list):
//   - head.next: CHANGED - becomes old a->head.next (or &b->head if a empty)
//   - head.prev: UNCHANGED
//   - tail.prev: CHANGED - becomes old a->tail.prev (or &b->tail if a empty)
//   - tail.next: UNCHANGED
//   - The first node's prev pointer: CHANGED to point to &b->head
//   - The last node's next pointer: CHANGED to point to &b->tail
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&a): YES (must hold after call)
//   - aws_linked_list_is_valid(&b): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness(void) {
    /* Allocate two linked lists with bounded lengths */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions: both lists must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old state */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last  = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last  = b.tail.prev;

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* === POST-CONDITIONS === */

    /* Both lists must still be valid after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Check a's new contents = b's old contents */
    if (b_was_empty) {
        /* a should now be empty */
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a should now contain b's old elements */
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == old_b_first);
        assert(a.tail.prev == old_b_last);
        /* The first node's prev should point back to a's head */
        assert(a.head.next->prev == &a.head);
        /* The last node's next should point to a's tail */
        assert(a.tail.prev->next == &a.tail);
    }

    /* Check b's new contents = a's old contents */
    if (a_was_empty) {
        /* b should now be empty */
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should now contain a's old elements */
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == old_a_first);
        assert(b.tail.prev == old_a_last);
        /* The first node's prev should point back to b's head */
        assert(b.head.next->prev == &b.head);
        /* The last node's next should point to b's tail */
        assert(b.tail.prev->next == &b.tail);
    }
}
