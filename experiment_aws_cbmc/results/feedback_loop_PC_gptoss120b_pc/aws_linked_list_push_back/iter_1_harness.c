#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

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
    struct aws_linked_list_node *old_last = list.tail.prev; /* previous last element (or head if empty) */

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Postconditions */

    /* 5.1. List validity invariant must hold */
    assert(aws_linked_list_is_valid(&list));

    /* 5.2. Sentinel invariants (head.prev == NULL, tail.next == NULL) must remain unchanged */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* 5.3. The new node becomes the last element */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_last);

    /* 5.4. The previous last element now points forward to the new node */
    assert(old_last->next == node);

    /* 5.5. The new node's linkage satisfies node validity predicates */
    /* (implicitly covered by the above asserts, but we state them explicitly) */
    assert(node->next->prev == node);
    assert(node->prev->next == node);
}
