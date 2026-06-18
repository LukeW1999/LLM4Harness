#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness(void) {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* 2. Create a node to push */
    struct aws_linked_list_node node;

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node *old_front = list.head.next;

    /* 3. Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* 4. Assert postconditions */

    /* list must be valid */
    assert(aws_linked_list_is_valid(&list));

    /* list must not be empty */
    assert(!aws_linked_list_empty(&list));

    /* node should be at the front */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);

    /* node's next should be the old front */
    assert(node.next == old_front);
    assert(old_front->prev == &node);

    /* head.prev must remain NULL (invariant) */
    assert(list.head.prev == NULL);
    /* tail.next must remain NULL (invariant) */
    assert(list.tail.next == NULL);
}
