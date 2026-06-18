#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rend_harness(void) {
    struct aws_linked_list list;

    /* Initialize the list (empty) */
    aws_linked_list_init(&list);

    /* Assume the list satisfies the structural validity predicate */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy of the list to check frame conditions later */
    struct aws_linked_list old = list;

    /* Call the function under test */
    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    /* 1. Return‑value correctness */
    assert(r == &list.head);

    /* 2. List must remain valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* 3. Frame condition – the list structure must be unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    return 0;
}
