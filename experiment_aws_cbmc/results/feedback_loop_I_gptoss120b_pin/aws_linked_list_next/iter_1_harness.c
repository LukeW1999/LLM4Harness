#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Choose a node that is guaranteed to be part of the list.
       Using the head sentinel satisfies the precondition that the node
       belongs to a valid list. */
    const struct aws_linked_list_node *node = &list.head;

    /* Save the original next pointer of the chosen node */
    const struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Postconditions */

    /* The function must return the original next pointer */
    assert(result == old_next);

    /* The list must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
