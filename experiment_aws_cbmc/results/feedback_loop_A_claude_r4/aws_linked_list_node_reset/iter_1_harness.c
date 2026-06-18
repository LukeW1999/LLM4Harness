#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    /* 1. Set up the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Set up the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    bool was_empty = aws_linked_list_empty(&list);

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* The new node is now the last element (tail.prev == node) */
    assert(list.tail.prev == node);

    /* node's next pointer points to tail */
    assert(node->next == &list.tail);

    /* node's prev pointer points to what was previously the last element */
    assert(node->prev == old_tail_prev);

    /* The old last element's next now points to node */
    assert(old_tail_prev->next == node);

    /* tail's prev points to node */
    assert(list.tail.prev == node);

    /* head.prev is still NULL (unchanged) */
    assert(list.head.prev == NULL);

    /* tail.next is still NULL (unchanged) */
    assert(list.tail.next == NULL);

    /* The list is no longer empty */
    assert(!aws_linked_list_empty(&list));

    /* The list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* node is now in the list */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
}
