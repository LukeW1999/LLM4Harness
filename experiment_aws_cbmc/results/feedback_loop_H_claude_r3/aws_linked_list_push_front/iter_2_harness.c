#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness(void) {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 2. Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Create a node to push */
    struct aws_linked_list_node node;

    /* 4. Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* 5. Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* 6. Assert postconditions */

    /* list must be valid */
    assert(aws_linked_list_is_valid(&list));

    /* list must not be empty */
    assert(!aws_linked_list_empty(&list));

    /* node should be at the front */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);

    /* node's next should be the old head next */
    assert(node.next == old_head_next);
    assert(old_head_next->prev == &node);

    /* head.prev must remain NULL */
    assert(list.head.prev == NULL);
    /* tail.next must remain NULL */
    assert(list.tail.next == NULL);
}
