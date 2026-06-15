#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aws/common/linked_list.h>

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* nondeterministically decide whether the list is empty or not */
    bool make_nonempty = __CPROVER_nondet_bool();
    if (make_nonempty) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&list, node);
    }

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list must be valid before call");

    struct aws_linked_list old = list;

    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    if (list.head.next == &list.tail) {
        /* Empty list */
        __CPROVER_assert(r == NULL, "rbegin should return NULL for empty list");
    } else {
        /* Non‑empty list */
        __CPROVER_assert(r == list.tail.prev, "rbegin should return last element");
    }

    /* List fields must be unchanged */
    __CPROVER_assert(list.head.next == old.head.next, "head.next unchanged");
    __CPROVER_assert(list.head.prev == old.head.prev, "head.prev unchanged");
    __CPROVER_assert(list.tail.next == old.tail.next, "tail.next unchanged");
    __CPROVER_assert(list.tail.prev == old.tail.prev, "tail.prev unchanged");

    /* Validity invariant must still hold */
    __CPROVER_assert(aws_linked_list_is_valid(&list), "list must remain valid after call");
}
