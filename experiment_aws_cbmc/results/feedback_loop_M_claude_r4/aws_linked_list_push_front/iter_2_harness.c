#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness() {
    /* Declare and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create a node to push */
    struct aws_linked_list_node node;

    /* Save old state */
    struct aws_linked_list_node *old_front = list.head.next;

    /* Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* Assert postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));

    /* node is now at the front */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_front);
    assert(old_front->prev == &node);
}
