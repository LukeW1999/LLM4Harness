#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate the first node */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    __CPROVER_assume(node1 != NULL);

    /* Push front on an empty list */
    aws_linked_list_push_front(&list, node1);

    /* After first push front */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == node1);
    assert(list.tail.prev == node1);
    assert(node1->prev == &list.head);
    assert(node1->next == &list.tail);

    /* Allocate a second node */
    struct aws_linked_list_node *node2 = malloc(sizeof(*node2));
    __CPROVER_assume(node2 != NULL);

    /* Save the previous head's next pointer (which is node1) */
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* Push front on a non-empty list */
    aws_linked_list_push_front(&list, node2);

    /* After second push front */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == node2);
    assert(node2->next == old_head_next);
    assert(node2->prev == &list.head);
    assert(old_head_next->prev == node2);
    assert(list.tail.prev == node1);
}
