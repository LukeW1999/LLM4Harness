#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

extern bool nondet_bool(void);

void aws_linked_list_begin_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    bool make_nonempty = nondet_bool();
    struct aws_linked_list_node *first = NULL;

    if (make_nonempty) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&list, node);
        first = node;
    }

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old_list = list;

    struct aws_linked_list_node *rval = aws_linked_list_begin(&list);

    if (aws_linked_list_empty(&list)) {
        assert(rval == &list.tail);
    } else {
        assert(rval == first);
        assert(rval->prev == &list.head);
        assert(aws_linked_list_node_next_is_valid(rval));
        assert(aws_linked_list_node_prev_is_valid(rval));
    }

    /* Frame condition: the list structure must not be modified. */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
}
