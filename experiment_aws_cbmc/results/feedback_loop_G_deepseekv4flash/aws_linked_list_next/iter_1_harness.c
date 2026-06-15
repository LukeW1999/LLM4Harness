#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* Create a non-empty valid linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Obtain a pointer to the first element (guaranteed real node because list is non-empty) */
    struct aws_linked_list_node *node = aws_linked_list_begin(&list);

    /* Save the original state of the node and the list for immutability checks */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list old_list = list;

    /* Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_next(node);

    /* 1. Postcondition: return value must equal node->next */
    assert(rval == node->next);

    /* 2. Unchanged fields on the node (const pointer, but verify no side effects) */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 3. Unchanged list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 4. List validity invariant preserved */
    assert(aws_linked_list_is_valid(&list));
}
