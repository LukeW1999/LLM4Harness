#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Choose a node that is in the list */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /* 3. Save old state */
    struct aws_linked_list old_list = list;
    const struct aws_linked_list_node *old_node = node;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Postconditions */
    /* Return value must be the next pointer of the input node */
    assert(result == node->next);

    /* No modifications to the list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* The input node pointer itself must not change */
    assert(node == old_node);

    /* 6. Invariants: the list remains valid after the call */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
