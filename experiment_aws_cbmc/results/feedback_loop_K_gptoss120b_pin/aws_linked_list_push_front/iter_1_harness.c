/*  
 * Contract for aws_linked_list_push_front  
 * ------------------------------------------------------------  
 * Preconditions:  
 *   - The list pointer `list` is non‑NULL and satisfies `aws_linked_list_is_valid(list)`.  
 *   - The node pointer `node` is non‑NULL and is not currently linked in any list  
 *     (i.e., `node->next == NULL && node->prev == NULL`).  
 *   - No aliasing: `node` does not point to any of the internal sentinel nodes of `list`.  
 *   - The allocator used for any dynamic allocation is the default allocator.  
 *   
 * Postconditions (validity):  
 *   - `aws_linked_list_is_valid(list)` holds after the call.  
 *   - The list is non‑empty.  
 *   - `list->head.next == node` (the new node is the first element).  
 *   - `node->prev == &list->head`.  
 *   - `node->next` points to the element that was previously first in the list  
 *     (or to `&list->tail` if the list was empty).  
 *   
 * Postconditions (length):  
 *   - Let `len_before` be the number of user nodes in `list` before the call.  
 *   - Let `len_after` be the number of user nodes after the call.  
 *   - `len_after == len_before + 1`.  
 *   
 * Postconditions (frame):  
 *   - All memory locations belonging to existing nodes (other than `node`) are unchanged  
 *     except for the `prev` field of the former first node, which now points to `node`.  
 *   - The sentinel nodes (`list->head` and `list->tail`) are only modified as required to  
 *     link the new node at the front.  
 * ------------------------------------------------------------  
 */

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

/* Helper to compute the length of a list */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    for (struct aws_linked_list_node *it = list->head.next; it != &list->tail; it = it->next) {
        len++;
    }
    return len;
}

/* Harness for aws_linked_list_push_front */
void aws_linked_list_push_front_harness(void) {
    /* Allocate and initialize the list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Build an arbitrary initial list (up to a bounded size) */
    const size_t MAX_INITIAL_NODES = 5;
    size_t len_before = 0;
    struct aws_linked_list_node *old_last = NULL; /* to capture tail.prev before the call */

    for (size_t i = 0; i < MAX_INITIAL_NODES; ++i) {
        if (__CPROVER_nondet_bool()) {
            struct aws_linked_list_node *n = malloc(sizeof(*n));
            __CPROVER_assume(n != NULL);
            aws_linked_list_node_reset(n);
            aws_linked_list_push_back(&list, n);
            len_before++;
        }
    }

    old_last = list.tail.prev; /* may be &list.head if the list is empty */

    /* Allocate the node to be pushed */
    struct aws_linked_list_node *new_node = malloc(sizeof(*new_node));
    __CPROVER_assume(new_node != NULL);
    /* Ensure the node is not linked anywhere */
    aws_linked_list_node_reset(new_node);

    /* Capture the former first node (may be the tail sentinel) */
    struct aws_linked_list_node *old_first = list.head.next;

    /* Call the function under test */
    aws_linked_list_push_front(&list, new_node);

    /* ---- Postcondition checks ---- */

    /* Validity of the list */
    assert(aws_linked_list_is_valid(&list));

    /* Length increased by exactly one */
    size_t len_after = list_length(&list);
    assert(len_after == len_before + 1);

    /* New node is now the first element */
    assert(list.head.next == new_node);
    assert(new_node->prev == &list.head);
    assert(new_node->next == old_first);

    /* Tail sentinel handling */
    if (len_before == 0) {
        /* List was empty, new node is also the last element */
        assert(list.tail.prev == new_node);
    } else {
        /* The previous last element must be unchanged */
        assert(list.tail.prev == old_last);
    }

    /* Frame condition: existing nodes (except possibly the former first) retain their next/prev links */
    for (struct aws_linked_list_node *it = list.head.next->next; it != &list.tail; it = it->next) {
        /* `it` is any node other than `new_node` and the former first node */
        assert(it->prev->next == it);
        assert(it->next->prev == it);
    }

    /* The former first node (if any) must now point back to the new node */
    if (old_first != &list.tail) {
        assert(old_first->prev == new_node);
    }
}
