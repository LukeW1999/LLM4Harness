#include <aws/common/common.h>
#include <aws/common/linked_list.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Initialize list as empty */
    list.head.prev = NULL;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;
    list.tail.next = NULL;

    /* Ensure the list is valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Node to add is not yet in the list */
    node.prev = NULL;
    node.next = NULL;

    /* Save old tail predecessor */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* Postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_tail_prev);

    if (old_tail_prev == &list.head) {
        assert(list.head.next == &node);
    } else {
        assert(old_tail_prev->next == &node);
    }

    /* Additional invariants */
    assert(list.tail.next == NULL);
    assert(list.head.prev == NULL);
}
