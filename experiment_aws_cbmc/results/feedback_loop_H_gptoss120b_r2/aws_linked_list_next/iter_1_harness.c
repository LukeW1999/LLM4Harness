#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_next */
void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node to query.
       For simplicity we use the list head sentinel, which is always present. */
    struct aws_linked_list_node *node = &list.head;

    /* 3. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Postconditions */

    /* The function must return the original next pointer */
    assert(result == old_next);

    /* No modifications to the list structure */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The node itself must remain unchanged */
    assert(node->next == old_next);
    assert(node->prev == old.head.prev); /* head.prev is unchanged (typically NULL) */

    /* 6. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
