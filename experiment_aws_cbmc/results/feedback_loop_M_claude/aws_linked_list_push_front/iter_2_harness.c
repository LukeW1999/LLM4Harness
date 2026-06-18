#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness() {
    /* Declare and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate a node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Save old state before calling */
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* Call function under test */
    aws_linked_list_push_front(&list, node);

    /* Assert postconditions */

    /* List must be valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* List must not be empty */
    assert(!aws_linked_list_empty(&list));

    /* The node should be at the front */
    assert(list.head.next == node);

    /* The node's prev should point to head */
    assert(node->prev == &list.head);

    /* The node's next should point to what was previously the first node */
    assert(node->next == old_head_next);

    /* The old first node's prev should now point to the new node */
    assert(old_head_next->prev == node);
}
