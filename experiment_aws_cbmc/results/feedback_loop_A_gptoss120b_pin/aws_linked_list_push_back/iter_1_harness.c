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
    struct aws_linked_list_node *old_last = list.tail.prev; /* could be &list.head if empty */

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Postconditions */

    /* The list must remain a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));

    /* Tail sentinel now points to the newly inserted node */
    assert(list.tail.prev == node);

    /* The new node links back to the previous last element */
    assert(node->prev == old_last);

    /* The new node links forward to the tail sentinel */
    assert(node->next == &list.tail);

    /* The previous last element now links forward to the new node */
    assert(old_last->next == node);

    /* Sentinel fields that must not change */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
