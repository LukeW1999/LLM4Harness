#include <assert.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_push_back_harness() {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node to be inserted */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state before the call */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Postconditions on success (function is void, so only one path) */
    /* List validity must hold */
    assert(aws_linked_list_is_valid(&list));

    /* The new node becomes the last element */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_last);
    assert(old_last->next == node);

    /* 6. Unchanged fields / invariants */
    assert(list.head.prev == NULL);   /* sentinel invariant */
    assert(list.tail.next == NULL);   /* sentinel invariant */

    /* Fields not mentioned as changing remain unchanged */
    assert(old.head.prev == list.head.prev);
    assert(old.tail.next == list.tail.next);
}
