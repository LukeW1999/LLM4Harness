#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Declare and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Pick a non-sentinel node to remove from the list */
    struct aws_linked_list_node *node = NULL;
    struct aws_linked_list_node *iter = list.head.next;
    while (iter != &list.tail) {
        if (nondet_bool()) {
            node = iter;
            break;
        }
        iter = iter->next;
    }
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head && node != &list.tail);

    /* Save the node's neighbours before removal */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call the function under test */
    aws_linked_list_remove(node);

    /* 4. Postconditions */

    /* The removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* The old neighbours now point to each other */
    if (old_prev != NULL) {
        assert(old_prev->next == old_next);
    }
    if (old_next != NULL) {
        assert(old_next->prev == old_prev);
    }

    /* Sentinel nodes that may have been adjacent are updated correctly */
    if (old_prev == &list.head) {
        assert(list.head.next == old_next);
    }
    if (old_next == &list.tail) {
        assert(list.tail.prev == old_prev);
    }

    /* The node is no longer part of any list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* The sentinel invariants remain intact */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* 5. Invariant: the list remains deeply valid */
    assert(aws_linked_list_is_valid_deep(&list));
}
