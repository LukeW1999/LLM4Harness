#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_linked_list_push_front_harness(void) {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 2. Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Create a node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 4. Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* 5. Call function under test */
    aws_linked_list_push_front(&list, node);

    /* 6. Assert postconditions */

    /* The list must be valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* The list must be non-empty after push */
    assert(!aws_linked_list_empty(&list));

    /* The node should now be at the front of the list */
    assert(list.head.next == node);

    /* The node's prev should point to the head sentinel */
    assert(node->prev == &list.head);

    /* The node's next should point to what was previously the first element */
    assert(node->next == old_head_next);

    /* The old first element's prev should now point to node */
    assert(old_head_next->prev == node);
}
