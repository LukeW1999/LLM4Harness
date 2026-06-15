#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_end_harness(void) {
    struct aws_linked_list list;

    /* nondet initialization of the list structure */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a snapshot of the list fields for the frame condition */
    struct aws_linked_list old_list = list;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* 1. Return value correctness */
    assert(result == &list.tail);
    assert(result != NULL);

    /* 2. List remains valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* 3. Frame condition: the list structure is unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    return 0;
}
