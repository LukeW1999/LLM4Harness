#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    /* Initialize a linked list with up to MAX_LINKED_LIST_ITEM_ALLOCATION nodes */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Get a pointer to the first element (or the tail sentinel if empty) */
    struct aws_linked_list_node *node = aws_linked_list_begin(&list);

    /* Save old state of the node's fields */
    struct aws_linked_list_node old_node = *node;

    /* Save list head.next and tail.prev for immutability check */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_prev(node);

    /* Postcondition: the function returns the previous node as recorded in node->prev */
    assert(rval == old_node.prev);

    /* The node's fields are unchanged (the function is const) */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* The list's sentinel pointers remain unchanged */
    assert(list.head.next == old_head_next);
    assert(list.tail.prev == old_tail_prev);

    /* The list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
