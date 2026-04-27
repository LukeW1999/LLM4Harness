// === STEP 1: SUCCESS PATH ===
// aws_linked_list_swap_contents is void, always "succeeds"
// After the call:
//   - a->head.next points to what was b's first node (or &a->tail if b was empty)
//   - a->tail.prev points to what was b's last node (or &a->head if b was empty)
//   - b->head.next points to what was a's first node (or &b->tail if a was empty)
//   - b->tail.prev points to what was a's last node (or &b->head if a was empty)
//   - Both lists remain valid linked lists
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void
//
// === STEP 3: FRAME CONDITIONS ===
// struct aws_linked_list *a:
//   - head.next: CHANGED (now points to b's old first node, or &a->tail)
//   - head.prev: UNCHANGED (head.prev is not used/modified)
//   - tail.prev: CHANGED (now points to b's old last node, or &a->head)
//   - tail.next: UNCHANGED (tail.next is not used/modified)
// struct aws_linked_list *b:
//   - head.next: CHANGED (now points to a's old first node, or &b->tail)
//   - head.prev: UNCHANGED
//   - tail.prev: CHANGED (now points to a's old last node, or &b->head)
//   - tail.next: UNCHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(a): YES (must hold after call)
//   - aws_linked_list_is_valid(b): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness() {
    /* Allocate two linked lists with bounded lengths */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions: both lists must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old state */
    struct aws_linked_list_node *a_old_first = a.head.next;
    struct aws_linked_list_node *a_old_last  = a.tail.prev;
    struct aws_linked_list_node *b_old_first = b.head.next;
    struct aws_linked_list_node *b_old_last  = b.tail.prev;

    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Post-conditions: both lists must still be valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Check that a now contains b's old contents */
    if (b_was_empty) {
        /* a should now be empty */
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a's first node should be b's old first node */
        assert(a.head.next == b_old_first);
        /* a's first node's prev should point back to a's head */
        assert(a.head.next->prev == &a.head);
        /* a's last node should be b's old last node */
        assert(a.tail.prev == b_old_last);
        /* a's last node's next should point to a's tail */
        assert(a.tail.prev->next == &a.tail);
    }

    /* Check that b now contains a's old contents */
    if (a_was_empty) {
        /* b should now be empty */
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b's first node should be a's old first node */
        assert(b.head.next == a_old_first);
        /* b's first node's prev should point back to b's head */
        assert(b.head.next->prev == &b.head);
        /* b's last node should be a's old last node */
        assert(b.tail.prev == a_old_last);
        /* b's last node's next should point to b's tail */
        assert(b.tail.prev->next == &b.tail);
    }
}
