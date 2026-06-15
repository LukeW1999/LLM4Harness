#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "aws/common/linked_list.h"

_Bool nondet_bool(void);

void aws_linked_list_insert_before_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *first = malloc(sizeof(*first));
    __CPROVER_assume(first != NULL);
    aws_linked_list_node_reset(first);
    aws_linked_list_push_back(&list, first);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *before;
    if (nondet_bool()) {
        before = &list.tail;               /* insert before tail sentinel */
    } else {
        before = list.head.next;           /* insert before first real element */
    }
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);

    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);    /* next == prev == NULL */

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_before_prev = before->prev;

    aws_linked_list_insert_before(before, to_add);

    /* Post‑conditions */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(aws_linked_list_node_is_in_list(to_add));
    assert(before->prev == to_add);
    if (old_before_prev != &list.head) {
        assert(old_before_prev->next == to_add);
    }
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
    assert(list.head.prev == old_list.head.prev);   /* should still be &list.head */
    assert(list.tail.next == old_list.tail.next);   /* should still be &list.tail */
}
