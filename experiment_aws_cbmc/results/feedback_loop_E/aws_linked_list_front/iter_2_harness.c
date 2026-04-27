#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_front_harness() {
    /* data structure */
    struct aws_linked_list list;

    /* Initialize the list */
    aws_linked_list_init(&list, NULL);

    /* Add some nodes to the list */
    struct aws_linked_list_node node1;
    struct aws_linked_list_node node2;
    aws_linked_list_push_back(&list, &node1);
    aws_linked_list_push_back(&list, &node2);

    /* Save old state */
    struct aws_linked_list old_list = list;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* assertions */
    if (!aws_linked_list_empty(&list)) {
        assert(result == list.head.next);
        assert(aws_linked_list_node_next_is_valid(result));
        assert(aws_linked_list_node_prev_is_valid(result));
    } else {
        assert(result == NULL);
    }

    /* unchanged fields */
    assert(list.head.prev == &list.tail);
    assert(list.tail.next == &list.head);

    /* validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
