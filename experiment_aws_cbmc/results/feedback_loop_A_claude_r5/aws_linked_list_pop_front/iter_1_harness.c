#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare the node to push */
    struct aws_linked_list_node node;

    /* 3. Save old state before calling */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    bool was_empty = aws_linked_list_empty(&list);

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 5. Assert postconditions */

    /* Changed: node is now the last element */
    assert(list.tail.prev == &node);

    /* Changed: node's next points to tail */
    assert(node.next == &list.tail);

    /* Changed: node's prev points to what was previously the last element */
    assert(node.prev == old_tail_prev);

    /* Changed: old last element's next now points to node */
    assert(old_tail_prev->next == &node);

    /* Unchanged: head's prev is still NULL */
    assert(list.head.prev == NULL);

    /* Unchanged: tail's next is still NULL */
    assert(list.tail.next == NULL);

    /* The list is no longer empty after push_back */
    assert(!aws_linked_list_empty(&list));

    /* Validity invariant */
    assert(aws_linked_list_is_valid(&list));

    /* Node is properly linked in the list */
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&node));
}
