#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_push_back_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a new node (precondition: node != NULL) */
    struct aws_linked_list_node *node = malloc(sizeof *node);
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Postconditions */

    /* List validity invariant must hold */
    assert(aws_linked_list_is_valid(&list));

    /* The new node becomes the last element */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_last);

    /* Head sentinel must still point to the first element */
    assert(list.head.next == node);
    /* Sentinel links that never change */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* Unchanged fields of the list (other than those modified above) */
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* Node linkage validity predicates */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
}
