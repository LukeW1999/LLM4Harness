#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness() {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a node to push */
    struct aws_linked_list_node node;

    /* 3. Save old state before calling */
    struct aws_linked_list_node *old_front = list.head.next;

    /* 4. Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* 5. Assert postconditions */

    /* list must be valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* list must not be empty */
    assert(!aws_linked_list_empty(&list));

    /* node should be at the front */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);

    /* node's next should be the old front */
    assert(node.next == old_front);
    assert(old_front->prev == &node);
}
