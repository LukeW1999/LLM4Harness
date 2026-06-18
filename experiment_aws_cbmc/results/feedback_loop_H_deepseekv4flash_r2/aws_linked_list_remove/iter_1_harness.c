#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#    define MAX_LINKED_LIST_ITEM_ALLOCATION 2
#endif

void aws_linked_list_remove_harness() {
    /* Create a linked list with up to MAX_LINKED_LIST_ITEM_ALLOCATION nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Choose a node to remove: either the first node after head or the second (if it exists) */
    struct aws_linked_list_node *node = NULL;
    struct aws_linked_list_node *first = list.head.next;
    if (first != &list.tail) {
        if (nondet_bool()) {
            node = first;
        } else {
            struct aws_linked_list_node *second = first->next;
            if (second != &list.tail) {
                node = second;
            } else {
                node = first; /* fallback when only one node */
            }
        }
    }
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head && node != &list.tail);

    /* Save the neighbors before removal */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* Postconditions */
    /* 1. The removed node's pointers are reset to NULL */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* 2. The neighbor pointers are correctly bypassed */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 3. The sentinel head.prev remains NULL and tail.next remains NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* 4. The list remains valid (deep validity checks all links) */
    assert(aws_linked_list_is_valid_deep(&list));
}
