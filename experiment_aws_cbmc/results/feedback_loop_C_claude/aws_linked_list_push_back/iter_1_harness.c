// === STEP 1: SUCCESS PATH ===
// aws_linked_list_push_back is void (no return value), always succeeds given valid preconditions.
// After the call:
//   - list->tail.prev: CHANGES to node (node is the new last element)
//   - node->next: CHANGES to &list->tail
//   - node->prev: CHANGES to old list->tail.prev
//   - old_last->next: CHANGES to node (the previously last node now points to node)
//   - list->head.prev: UNCHANGED (always NULL per validity)
//   - list->tail.next: UNCHANGED (always NULL per validity)
//   - list->head.next: UNCHANGED (unless list was empty, but still valid)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always succeeds given valid preconditions.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - list->head.prev: UNCHANGED (NULL always)
//   - list->head.next: UNCHANGED (unless list was empty)
//   - list->tail.next: UNCHANGED (NULL always)
//   - list->tail.prev: CHANGED to node
// node (struct aws_linked_list_node):
//   - node->next: CHANGED to &list->tail
//   - node->prev: CHANGED to old tail.prev
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold after call)
//   - node->next->prev == node: YES (node_next_is_valid)
//   - node->prev->next == node: YES (node_prev_is_valid)
//   - list->tail.prev == node: YES (node is new last element)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness(void) {
    /* Allocate and initialize a valid linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate a new node to push back */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Save old state before the call */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* === STEP 1 assertions: post-call state === */
    /* node is the new last element */
    assert(list.tail.prev == node);
    /* node->next points to tail sentinel */
    assert(node->next == &list.tail);
    /* node->prev points to what was previously the last node */
    assert(node->prev == old_tail_prev);
    /* The previously last node now points forward to node */
    assert(old_tail_prev->next == node);

    /* === STEP 3 assertions: frame conditions === */
    /* head.prev is always NULL (sentinel invariant) */
    assert(list.head.prev == old_head_prev);
    assert(list.head.prev == NULL);
    /* tail.next is always NULL (sentinel invariant) */
    assert(list.tail.next == old_tail_next);
    assert(list.tail.next == NULL);

    /* === STEP 4 assertions: validity invariants === */
    /* Overall list validity */
    assert(aws_linked_list_is_valid(&list));
    /* node->next->prev == node */
    assert(node->next->prev == node);
    /* node->prev->next == node */
    assert(node->prev->next == node);
    /* Postcondition from spec: list->tail.prev == node */
    assert(list.tail.prev == node);
}
