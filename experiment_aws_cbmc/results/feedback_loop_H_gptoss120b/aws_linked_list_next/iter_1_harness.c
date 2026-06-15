#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is not the list tail (tail's next is undefined) */
    const struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.tail); /* node must have a valid next pointer */

    /* 3. Save old state */
    struct aws_linked_list old = list;
    const struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Postconditions */

    /* The returned pointer must be the original next pointer of the node */
    assert(result == old_next);

    /* The node's next pointer must remain unchanged */
    assert(node->next == old_next);

    /* The list structure must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
