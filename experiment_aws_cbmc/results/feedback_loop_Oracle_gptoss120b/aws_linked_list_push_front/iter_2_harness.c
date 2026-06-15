#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_front_harness(void) {
    /* Initialize an empty, valid linked list. */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate a fresh node to be pushed to the front. */
    struct aws_linked_list_node *new_node = malloc(sizeof *new_node);
    __CPROVER_assume(new_node != NULL);
    aws_linked_list_node_reset(new_node);
    __CPROVER_assume(new_node->next == NULL && new_node->prev == NULL);

    /* Call the function under verification. */
    aws_linked_list_push_front(&list, new_node);

    /* Post‑conditions: the list must remain valid and the new node must be the
       first (and only) element. */
    assert(aws_linked_list_is_valid(&list));

    assert(list.head.next == new_node);
    assert(new_node->prev == &list.head);
    assert(new_node->next == &list.tail);
    assert(list.tail.prev == new_node);

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
