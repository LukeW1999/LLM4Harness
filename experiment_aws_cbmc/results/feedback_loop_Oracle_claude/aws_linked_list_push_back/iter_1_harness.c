#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Validity predicates (mirroring the inline definitions)
 */
static bool aws_linked_list_node_next_is_valid(const struct aws_linked_list_node *node) {
    return node && node->next && node->next->prev == node;
}

static bool aws_linked_list_node_prev_is_valid(const struct aws_linked_list_node *node) {
    return node && node->prev && node->prev->next == node;
}

static bool aws_linked_list_is_valid(const struct aws_linked_list *list) {
    if (!list) return false;
    /* head.prev == NULL, tail.next == NULL */
    if (list->head.prev != NULL) return false;
    if (list->tail.next != NULL) return false;
    /* head <-> ... <-> tail: head.next->prev == &head and tail.prev->next == &tail */
    if (!list->head.next) return false;
    if (!list->tail.prev) return false;
    if (list->head.next->prev != &list->head) return false;
    if (list->tail.prev->next != &list->tail) return false;
    return true;
}

/**
 * Inline implementation of insert_before (needed for the harness to compile
 * without the real headers providing the inline bodies).
 */
static void aws_linked_list_insert_before(
    struct aws_linked_list_node *next_node,
    struct aws_linked_list_node *node) {
    node->next = next_node;
    node->prev = next_node->prev;
    node->prev->next = node;
    next_node->prev = node;
}

/**
 * Inline implementation of push_back.
 */
static void aws_linked_list_push_back(
    struct aws_linked_list *list,
    struct aws_linked_list_node *node) {
    aws_linked_list_insert_before(&list->tail, node);
}

/**
 * CBMC proof harness for aws_linked_list_push_back.
 */
void aws_linked_list_push_back_harness(void) {
    /* Allocate and initialise a valid linked list (empty: head <-> tail) */
    struct aws_linked_list *list = malloc(sizeof(*list));
    if (!list) return;

    /* Set up the sentinel structure for an empty list */
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;

    /* Precondition: list must be valid before the call */
    assert(aws_linked_list_is_valid(list));

    /* Allocate a new node (non-deterministic content is fine) */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    if (!node) return;

    /* Precondition: node must be non-NULL */
    assert(node != NULL);

    /* Remember the old last element (the node that was just before tail) */
    struct aws_linked_list_node *old_last = list->tail.prev;

    /* Call the function under verification */
    aws_linked_list_push_back(list, node);

    /* ---------------------------------------------------------------
     * Postcondition 1: list validity is preserved
     * --------------------------------------------------------------- */
    assert(aws_linked_list_is_valid(list));

    /* ---------------------------------------------------------------
     * Postcondition 2: node is now the last element (just before tail)
     * --------------------------------------------------------------- */
    assert(list->tail.prev == node);

    /* ---------------------------------------------------------------
     * Postcondition 3: node's next pointer points to tail
     * --------------------------------------------------------------- */
    assert(node->next == &list->tail);

    /* ---------------------------------------------------------------
     * Postcondition 4: node's prev pointer points to the old last node
     * --------------------------------------------------------------- */
    assert(node->prev == old_last);

    /* ---------------------------------------------------------------
     * Postcondition 5: the old last node's next now points to node
     *   (frame condition: only the affected pointers were modified)
     * --------------------------------------------------------------- */
    assert(old_last->next == node);

    /* ---------------------------------------------------------------
     * Postcondition 6: node->next->prev == node  (next_is_valid for node)
     * --------------------------------------------------------------- */
    assert(aws_linked_list_node_next_is_valid(node));

    /* ---------------------------------------------------------------
     * Postcondition 7: node->prev->next == node  (prev_is_valid for node)
     * --------------------------------------------------------------- */
    assert(aws_linked_list_node_prev_is_valid(node));

    /* ---------------------------------------------------------------
     * Postcondition 8: tail's prev_is_valid (tail.prev->next == &tail)
     * --------------------------------------------------------------- */
    assert(aws_linked_list_node_prev_is_valid(&list->tail));

    /* ---------------------------------------------------------------
     * Postcondition 9: head sentinel is untouched
     *   head.prev is still NULL and head.next->prev == &head
     * --------------------------------------------------------------- */
    assert(list->head.prev == NULL);
    assert(list->head.next->prev == &list->head);

    /* ---------------------------------------------------------------
     * Postcondition 10: tail.next is still NULL (sentinel invariant)
     * --------------------------------------------------------------- */
    assert(list->tail.next == NULL);
}
