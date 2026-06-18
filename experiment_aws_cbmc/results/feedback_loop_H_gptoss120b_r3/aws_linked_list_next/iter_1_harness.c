#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is in the list */
    struct aws_linked_list_node *node;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /* 3. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node = *node;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Postconditions */
    /* The function returns the next pointer of the supplied node */
    assert(result == node->next);

    /* No fields of the list change */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* No fields of the node change */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 6. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
