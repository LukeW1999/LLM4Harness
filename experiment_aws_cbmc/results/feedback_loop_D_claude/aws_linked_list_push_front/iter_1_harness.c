// === STEP 1: SUCCESS PATH ===
// aws_linked_list_push_front is void, no return value.
// On success (preconditions met):
//   - list->head.next: CHANGES to node (node becomes the new first element)
//   - list->head.prev: UNCHANGED (remains NULL)
//   - list->tail.prev: UNCHANGED (unless list was empty, but even then it points to node via insert_before)
//   - list->tail.next: UNCHANGED (remains NULL)
//   - node->next: CHANGES to old list->head.next (the previous first element or tail)
//   - node->prev: CHANGES to &list->head
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void with preconditions only.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: CHANGED (points to node after call)
//   - head.prev: UNCHANGED (always NULL)
//   - tail.prev: UNCHANGED (unless list was empty; node->next points to tail in that case)
//   - tail.next: UNCHANGED (always NULL)
// node (struct aws_linked_list_node):
//   - next: CHANGED (points to old head.next)
//   - prev: CHANGED (points to &list->head)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold after call)
//   - list->head.next == node: YES (node is new front)
//   - node->prev == &list->head: YES
//   - node->next == old_head_next: YES
//   - old_head_next->prev == node: YES
//   - list is not empty after call: YES

#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_front_harness(void) {
    // Allocate and initialize a linked list with some bounded number of elements
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    // Allocate a new node to push to the front
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    // Save old state before the call
    struct aws_linked_list_node *old_head_next = list.head.next;

    // Call the function under test
    aws_linked_list_push_front(&list, node);

    // === STEP 1 assertions: verify the new state ===
    // node is now the first element
    assert(list.head.next == node);
    // node's prev points to head
    assert(node->prev == &list.head);
    // node's next points to what was previously the first element
    assert(node->next == old_head_next);
    // the old first element's prev now points to node
    assert(old_head_next->prev == node);

    // === STEP 3 assertions: frame conditions ===
    // head.prev is always NULL
    assert(list.head.prev == NULL);
    // tail.next is always NULL
    assert(list.tail.next == NULL);

    // === STEP 4 assertions: validity invariants ===
    assert(aws_linked_list_is_valid(&list));
    // List is not empty after push_front
    assert(!aws_linked_list_empty(&list));
    // node is properly linked (both directions valid)
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
}
