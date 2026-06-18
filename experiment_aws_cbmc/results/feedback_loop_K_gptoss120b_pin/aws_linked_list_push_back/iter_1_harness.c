/*=== Contract ===
Preconditions:
  - The list pointed to by `list` satisfies aws_linked_list_is_valid:
        list->head.prev == NULL
        list->tail.next == NULL
        list->head.next != NULL && list->head.next->prev == &list->head
        list->tail.prev != NULL && list->tail.prev->next == &list->tail
  - `node` is non‑NULL.

Postconditions (validity):
  - After the call, `list` still satisfies aws_linked_list_is_valid.
  - `list->tail.prev == node` (the pushed node becomes the new last element).

Postconditions (structure):
  - `node->next == &list->tail`.
  - `node->prev` equals the previous last element (`old_tail_prev`).
  - If the previous last element was not the head sentinel, then
        `old_tail_prev->next == node`.

Postconditions (frame):
  - No other memory locations are modified (aside from the nodes directly
    involved in the insertion). ===*/

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "aws/common/linked_list.h"
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_push_back_harness(void) {
    /* Allocate and set up a valid linked list (empty list) */
    struct aws_linked_list list;
    /* Sentinel head */
    list.head.prev = NULL;
    list.head.next = &list.tail;
    /* Sentinel tail */
    list.tail.prev = &list.head;
    list.tail.next = NULL;

    /* Save the previous last element (the head sentinel in the empty case) */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Allocate a node to push */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* Postcondition: list remains valid */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next != NULL);
    assert(list.head.next->prev == &list.head);
    assert(list.tail.prev != NULL);
    assert(list.tail.prev->next == &list.tail);

    /* Postcondition: node is now the last element */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_tail_prev);
    if (old_tail_prev != &list.head) {
        assert(old_tail_prev->next == node);
    }

    return 0;
}
