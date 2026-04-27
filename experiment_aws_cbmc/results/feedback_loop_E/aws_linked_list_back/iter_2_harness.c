#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness() {
    /* data structure */
    struct aws_linked_list list;

    /* Initialize the linked list */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old_list = list;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* assertions */
    if (!aws_linked_list_empty(&list)) {
        assert(result == list.tail.prev);
        assert(aws_linked_list_node_prev_is_valid(result));
        assert(aws_linked_list_node_next_is_valid(result));
    } else {
        assert(result == NULL);
    }

    /* unchanged fields */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(list.head.prev == &list.tail);
    assert(list.tail.next == &list.head);

    /* validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
