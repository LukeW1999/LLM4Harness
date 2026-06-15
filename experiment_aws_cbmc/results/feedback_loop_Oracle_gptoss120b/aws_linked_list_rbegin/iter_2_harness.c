#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    struct aws_linked_list_node *last = NULL;
    if (__CPROVER_nondet_bool()) {
        last = malloc(sizeof(*last));
        __CPROVER_assume(last != NULL);
        aws_linked_list_push_back(&list, last);
    }

    struct aws_linked_list list_old = list;

    struct aws_linked_list_node *rbegin = aws_linked_list_rbegin(&list);

    assert(rbegin == list.tail.prev);

    if (aws_linked_list_empty(&list)) {
        assert(rbegin == &list.head);
    } else {
        assert(rbegin != &list.head);
        assert(rbegin == last);
        assert(aws_linked_list_node_prev_is_valid(rbegin));
    }

    assert(list.head.next == list_old.head.next);
    assert(list.head.prev == list_old.head.prev);
    assert(list.tail.next == list_old.tail.next);
    assert(list.tail.prev == list_old.tail.prev);
}
