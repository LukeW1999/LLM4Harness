#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* Create and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Allocate two nodes: one to be inserted before, and the node to add */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(node1 != NULL && to_add != NULL);

    /* Add node1 to the list (now list has one element) */
    aws_linked_list_push_back(&list, node1);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
    assert(node1->prev == &list.head);
    assert(node1->next == &list.tail);

    /* Save pointer states for postcondition checks */
    struct aws_linked_list_node *old_node1_prev = node1->prev;
    struct aws_linked_list_node *old_node1_next = node1->next;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Insert to_add before node1 */
    aws_linked_list_insert_before(node1, to_add);

    /* Postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* to_add should be before node1 */
    assert(to_add->next == node1);
    assert(to_add->prev == old_node1_prev);  /* which is &list.head */

    /* node1's prev now points to to_add */
    assert(node1->prev == to_add);

    /* head.next now points to to_add (since node1 was the only element) */
    assert(list.head.next == to_add);

    /* node1 is still the last element, so tail.prev should still be node1 */
    assert(list.tail.prev == node1);

    /* node1's next unchanged (still tail) */
    assert(node1->next == old_node1_next);

    /* Sentinel pointers unchanged */
    assert(list.tail.next == NULL);
    assert(list.head.prev == NULL);

    /* to_add's pointers correct */
    assert(to_add->prev == &list.head);
    assert(to_add->next == node1);
}
