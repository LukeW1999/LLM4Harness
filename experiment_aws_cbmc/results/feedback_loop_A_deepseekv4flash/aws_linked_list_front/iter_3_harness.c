#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_front_harness() {
    /* Initialize an empty list with dummy head/tail */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate a node and add it to the list to make it non-empty */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    /* Insert the node after the head */
    aws_linked_list_insert_after(&list.head, node);
    /* Now list.head.next == node, and list.tail.prev == node */

    /* Optionally, allow further allocation to make proof more robust */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result != NULL);
    assert(result == list.head.next);
    assert(result == old_head_next);
    /* List should be unchanged */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == &list.head);
    assert(list.tail.next == &list.tail);
    assert(list.tail.prev == node);
}
