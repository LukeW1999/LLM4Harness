#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness() {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node and insert it into the list */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_push_back(&list, node);

    /* 3. Save old state before the call */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Postconditions */

    /* The removed node's pointers are reset to NULL */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* The surrounding nodes are linked together */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* List structural fields that should not change */
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* The list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));
}
