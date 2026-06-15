#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_front_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* nondeterministically make the list empty to hit both branches */
    _Bool make_empty = __CPROVER_nondet_bool();
    if (make_empty) {
        list.head.next = &list.tail;
        list.tail.prev = &list.head;
        list.head.prev = &list.head;   /* maintain a well‑formed sentinel */
        list.tail.next = &list.tail;
        __CPROVER_assume(aws_linked_list_is_valid(&list));
    }

    struct aws_linked_list old = list;

    struct aws_linked_list_node *rval = aws_linked_list_front(&list);

    if (old.head.next == &old.tail) {
        __CPROVER_assert(rval == NULL,
                         "aws_linked_list_front should return NULL for an empty list");
    } else {
        __CPROVER_assert(rval == old.head.next,
                         "aws_linked_list_front should return the first node for a non‑empty list");
    }

    __CPROVER_assert(list.head.next == old.head.next,
                     "list.head.next must remain unchanged");
    __CPROVER_assert(list.head.prev == old.head.prev,
                     "list.head.prev must remain unchanged");
    __CPROVER_assert(list.tail.next == old.tail.next,
                     "list.tail.next must remain unchanged");
    __CPROVER_assert(list.tail.prev == old.tail.prev,
                     "list.tail.prev must remain unchanged");

    __CPROVER_assert(aws_linked_list_is_valid(&list),
                     "list must remain valid after aws_linked_list_front");
}
