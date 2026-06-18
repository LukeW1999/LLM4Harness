#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_front_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate at least one node and add it to the list */
    struct aws_linked_list_node *node1 = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node1 != NULL);
    aws_linked_list_node_reset(node1);
    aws_linked_list_push_back(&list, node1);

    /* Optionally add a second node */
    if (__CPROVER_nondet_bool()) {
        struct aws_linked_list_node *node2 = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node2 != NULL);
        aws_linked_list_node_reset(node2);
        aws_linked_list_push_back(&list, node2);
    }

    /* Precondition: list is not empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Snapshot the list structure for frame condition checks */
    struct aws_linked_list list_old = list;

    /* Call the function under verification */
    struct aws_linked_list_node *front = aws_linked_list_front(&list);

    /* Postcondition 1: Return value correctness */
    assert(front != NULL);
    assert(front->prev == &list.head);
    assert(list.head.next == front);
    assert(front != &list.tail);

    /* Postcondition 2: List validity remains */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 3: Frame condition (list structure unchanged) */
    assert(list.head.next == list_old.head.next);
    assert(list.head.prev == list_old.head.prev);
    assert(list.tail.next == list_old.tail.next);
    assert(list.tail.prev == list_old.tail.prev);

    return 0;
}
