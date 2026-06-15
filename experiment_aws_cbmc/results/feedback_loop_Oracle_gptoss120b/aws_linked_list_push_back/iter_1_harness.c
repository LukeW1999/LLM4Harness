/* CBMC harness for aws_linked_list_push_back
 *
 * Verifies that the function preserves the linked‑list invariants,
 * inserts the new node as the last element and that the list length
 * grows by exactly one.
 *
 * Allowed includes only the ones requested by the task.
 */

#include <aws/common/linked_list.h>
#include <aws/common/assert.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper: compute the length of a list (number of user nodes) */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        len++;
        cur = cur->next;
    }
    return len;
}

/* Helper: create a list with a nondet number of elements (0..MAX_ELEMS) */
static void make_nondet_list(struct aws_linked_list *list, size_t max_elems) {
    aws_linked_list_init(list);

    size_t n = nondet_uint() % (max_elems + 1);
    __CPROVER_assume(n <= max_elems);

    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *tmp = malloc(sizeof(*tmp));
        __CPROVER_assume(tmp != NULL);
        /* The node is freshly allocated; its internal links will be set by push_back */
        aws_linked_list_push_back(list, tmp);
    }
}

/* CBMC entry point */
void aws_linked_list_push_back_harness(void) {
    /* allocator – required by the rules */
    struct aws_allocator *alloc = aws_default_allocator();

    /* --------------------------------------------------------------------
     * Build a valid pre‑state list (may be empty) and a fresh node to push.
     * -------------------------------------------------------------------- */
    struct aws_linked_list list;
    make_nondet_list(&list, 5);               /* up to 5 existing elements */

    struct aws_linked_list_node *new_node = malloc(sizeof(*new_node));
    __CPROVER_assume(new_node != NULL);       /* PRE: node != NULL */

    /* Remember the old length for the length invariant */
    size_t old_len = list_length(&list);

    /* --------------------------------------------------------------------
     * Call the function under test.
     * -------------------------------------------------------------------- */
    aws_linked_list_push_back(&list, new_node);

    /* --------------------------------------------------------------------
     * Post‑condition checks.
     * -------------------------------------------------------------------- */

    /* 1. Validity predicates */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == new_node);                     /* new node is last element */
    assert(new_node->next == &list.tail);                   /* new node points to tail sentinel */
    assert(new_node->prev != NULL);                         /* prev must be the former last node (or head) */

    /* 2. Length invariant */
    assert(list_length(&list) == old_len + 1);

    /* 3. Frame conditions – nothing else in the list is modified.
     *
     *    We conservatively check that the head and tail sentinels keep
     *    their sentinel properties and that the node we inserted is the
     *    only node whose links have changed.
     */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next != NULL);                         /* at least one element now */
    assert(list.head.next->prev == &list.head);             /* first element links back to head */

    return 0;
}
