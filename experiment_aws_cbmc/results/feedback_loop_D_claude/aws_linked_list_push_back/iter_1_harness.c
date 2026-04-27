// === STEP 1: SUCCESS PATH ===
// aws_linked_list_push_back always succeeds (void return, no error path).
// After the call:
//   - list->tail.prev: CHANGES to node (node is now last element)
//   - node->next: CHANGES to &list->tail
//   - node->prev: CHANGES to old list->tail.prev
//   - old_last->next (the node that was previously last): CHANGES to node
//   - list->head.prev: UNCHANGED (always NULL per validity)
//   - list->tail.next: UNCHANGED (always NULL per validity)
//   - list->head.next: UNCHANGED (unless list was empty, but structurally head.next is unchanged if list had elements; if empty, head.next was &tail, now head.next == node... wait)
//   Actually if list was empty: head.next == &tail, tail.prev == &head
//   After push_back: head.next == node (because old tail.prev was &head, so node->prev = &head, head.next = node), tail.prev == node
//   So head.next CHANGES when list was empty.
//   In general: the node previously pointed to by tail.prev has its next changed to node.
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always executes.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - list->tail.prev: CHANGED to node
//   - list->head.next: CHANGED (if list was empty, head.next changes from &tail to node; if not empty, head.next unchanged)
//     Actually head.next is only changed if the old tail.prev was &head (empty list).
//     We can't easily assert head.next unchanged in general. We skip asserting it unchanged.
//   - list->head.prev: UNCHANGED (NULL)
//   - list->tail.next: UNCHANGED (NULL)
// node (struct aws_linked_list_node):
//   - node->next: CHANGED to &list->tail
//   - node->prev: CHANGED to old_tail_prev
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (postcondition)
//   - list->tail.prev == node: YES (postcondition)
//   - node->next == &list->tail: YES
//   - node->prev->next == node: YES (aws_linked_list_node_next_is_valid for node->prev)
//   - node->next->prev == node: YES (i.e., list->tail.prev == node)

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

/* Minimal includes to get the linked list types and functions */
#include <aws/common/linked_list.h>

void aws_linked_list_push_back_harness(void) {
    /* Allocate and initialize a valid linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate a new node (non-null, precondition) */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Save old state before the call */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    /* head.prev and tail.next should be NULL per validity */

    /* Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* === STEP 1 assertions: postconditions === */

    /* node is now the last element */
    assert(list.tail.prev == node);

    /* node->next points to tail sentinel */
    assert(node->next == &list.tail);

    /* node->prev points to what was previously the last node */
    assert(node->prev == old_tail_prev);

    /* The previously-last node's next now points to node */
    assert(old_tail_prev->next == node);

    /* === STEP 3: frame conditions === */

    /* head.prev is still NULL (validity invariant) */
    assert(list.head.prev == NULL);

    /* tail.next is still NULL (validity invariant) */
    assert(list.tail.next == NULL);

    /* === STEP 4: validity invariants === */

    /* The list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* node->next->prev == node (aws_linked_list_node_next_is_valid) */
    assert(node->next->prev == node);

    /* node->prev->next == node (aws_linked_list_node_prev_is_valid) */
    assert(node->prev->next == node);
}
