#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    /* Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate two nodes */
    struct aws_linked_list_node *node1 = malloc(sizeof *node1);
    struct aws_linked_list_node *node2 = malloc(sizeof *node2);
    __CPROVER_assume(node1 != NULL);
    __CPROVER_assume(node2 != NULL);

    /* Initialize node fields (push will set them) */
    node1->next = NULL;
    node1->prev = NULL;
    node2->next = NULL;
    node2->prev = NULL;

    /* Insert nodes into the list */
    aws_linked_list_push_back(&list, node1);
    aws_linked_list_push_back(&list, node2);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node2 = *node2;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node2);

    /* Postconditions */
    assert(result == old_node2.prev);
    assert(node2->next == old_node2.next);
    assert(node2->prev == old_node2.prev);
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
