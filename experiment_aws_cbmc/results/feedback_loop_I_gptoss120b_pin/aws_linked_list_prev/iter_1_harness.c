#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate a node and its previous node */
    struct aws_linked_list_node *node = malloc(sizeof *node);
    __CPROVER_assume(node != NULL);
    struct aws_linked_list_node *prev_node = malloc(sizeof *prev_node);
    __CPROVER_assume(prev_node != NULL);

    /* Non-deterministically set the next pointer (not used) */
    node->next = NULL;
    node->prev = prev_node;

    /* Save old state */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list_node old_prev_node = *prev_node;
    struct aws_linked_list old_list = list;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postconditions */
    assert(result == old_node.prev);                     /* returned pointer equals original prev */
    assert(node->next == old_node.next);                 /* node->next unchanged */
    assert(node->prev == old_node.prev);                 /* node->prev unchanged */
    assert(prev_node->next == old_prev_node.next);       /* prev_node unchanged */
    assert(prev_node->prev == old_prev_node.prev);       /* prev_node unchanged */

    /* List must remain valid and unchanged */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
}
