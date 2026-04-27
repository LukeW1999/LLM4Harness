#include <aws/common/linked_list.h>
#include <cbmc/model_assert.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare data structures */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* 2. Bound and allocate structures */
    __CPROVER_assume(&list != NULL);
    __CPROVER_assume(&node != NULL);

    /* Initialize sentinel nodes */
    list.head.next = &list.tail;
    list.head.prev = NULL;
    list.tail.prev = &list.head;
    list.tail.next = NULL;

    /* 3. Assume preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(&node != NULL);

    /* 4. Save old state if needed */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 5. Call function under verification */
    aws_linked_list_push_back(&list, &node);

    /* 6. Assert postconditions (both success and failure branches) */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(old_last->next == &node);
    assert(node.next == &list.tail);

    /* 7. Assert invariants (validity predicates) */
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
    assert(aws_linked_list_node_next_is_valid(&list.head));
}
