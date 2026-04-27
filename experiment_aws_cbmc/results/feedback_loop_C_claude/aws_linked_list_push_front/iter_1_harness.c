// === STEP 1: SUCCESS PATH ===
// aws_linked_list_push_front is void, so no return value.
// On success (always, given valid preconditions):
//   - list->head.next: CHANGES to node
//   - list->head.prev: UNCHANGED (remains NULL)
//   - list->tail.prev: UNCHANGED (unless list was empty, then it was already pointing to head, now points to node... wait)
//   Actually: push_front calls insert_before(list->head.next, node)
//   insert_before sets: node->next = before (old head.next), node->prev = before->prev (= &list->head)
//   before->prev->next = node (i.e., head.next = node), before->prev = node
//   So: list->head.next = node, node->prev = &list->head, node->next = old_head_next
//   list->tail.prev: UNCHANGED (tail.prev still points to whatever it was before)
//   list->head.prev: UNCHANGED (remains NULL)
//   list->tail.next: UNCHANGED (remains NULL)
//
// === STEP 2: FAILURE PATH ===
// Function is void, no failure path. Preconditions must hold.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: CHANGED to node
//   - head.prev: UNCHANGED (NULL)
//   - tail.prev: UNCHANGED (still points to old last element, or &list->head if was empty)
//     Wait - if list was empty: tail.prev was &list->head, after push_front: 
//     insert_before(list->head.next = &list->tail, node):
//       node->next = &list->tail, node->prev = &list->tail->prev = &list->head
//       list->tail->prev->next = node => list->head.next = node
//       list->tail->prev = node
//     So tail.prev CHANGES to node when list was empty!
//     In general: tail.prev changes only if list was empty (head.next == &list->tail before call)
//     Actually insert_before(before, node): before->prev = node. 
//     before = old head.next. If list was empty, before = &list->tail, so tail.prev = node.
//     If list was not empty, before = some_node (not tail), so tail.prev unchanged.
//   - tail.next: UNCHANGED (NULL)
// node (struct aws_linked_list_node):
//   - next: CHANGES to old list->head.next
//   - prev: CHANGES to &list->head
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold after call)
//   - list->head.next == node: YES
//   - node->prev == &list->head: YES
//   - node->next == old_head_next: YES

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness(void) {
    /* Allocate and initialize a valid linked list with bounded length */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate a new node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    bool was_empty = aws_linked_list_empty(&list);

    /* Call the function under test */
    aws_linked_list_push_front(&list, node);

    /* === STEP 1 assertions: state after call === */
    /* list->head.next must be node */
    assert(list.head.next == node);

    /* node->prev must be &list->head */
    assert(node->prev == &list->head);

    /* node->next must be old head.next */
    assert(node->next == old_head_next);

    /* list->head.prev must remain NULL */
    assert(list.head.prev == NULL);

    /* list->tail.next must remain NULL */
    assert(list.tail.next == NULL);

    /* tail.prev: if list was empty, tail.prev changes to node; otherwise unchanged */
    if (was_empty) {
        assert(list.tail.prev == node);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    /* === STEP 4: validity invariants === */
    assert(aws_linked_list_is_valid(&list));

    /* List must not be empty after push_front */
    assert(!aws_linked_list_empty(&list));

    /* node is properly linked */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
}
