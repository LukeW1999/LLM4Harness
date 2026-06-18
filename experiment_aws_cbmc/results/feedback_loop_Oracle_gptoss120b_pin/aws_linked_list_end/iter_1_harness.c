#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_linked_list_end_harness(void) {
    struct aws_linked_list list;

    /* nondet initialize the list structure */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy of the list to check frame conditions later */
    struct aws_linked_list old = list;

    /* Call the function under verification */
    const struct aws_linked_list_node *ret = aws_linked_list_end(&list);

    /* Postcondition 1: return value correctness */
    assert(ret == &list.tail);

    /* Postcondition 2: list remains valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 3: frame condition – the list contents are unchanged */
    assert(old.head.next == list.head.next);
    assert(old.head.prev == list.head.prev);
    assert(old.tail.next == list.tail.next);
    assert(old.tail.prev == list.tail.prev);

    return 0;
}
