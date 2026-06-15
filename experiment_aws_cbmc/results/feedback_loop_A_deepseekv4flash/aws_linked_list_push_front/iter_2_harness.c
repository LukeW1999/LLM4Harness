#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate the first node */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    __CPROVER_assume(node1 != NULL);

    /* Push back on an empty list */
    aws_linked_list_push_back(&list, node1);

    /* After first push */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == node1);
    assert(list.tail.prev == node1);
    assert(node1->prev == &list.head);
    assert(node1->next == &list.tail);

    /* Allocate a second node */
    struct aws_linked_list_node *node2 = malloc(sizeof(*node2));
    __CPROVER_assume(node2 != NULL);

    /* Save the previous tail's previous pointer (which is node1) */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Push back on a non-empty list */
    aws_linked_list_push_back(&list, node2);

    /* After second push */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == node2);
    assert(node2->next == &list.tail);
    assert(node2->prev == node1);
    assert(node1->next == node2);
    assert(list.head.next == node1);
}
