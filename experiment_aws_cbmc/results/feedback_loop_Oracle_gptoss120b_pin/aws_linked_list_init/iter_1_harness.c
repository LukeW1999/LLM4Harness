#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC harness for aws_linked_list_init */
void aws_linked_list_init_harness(void) {
    /* Allocate the list to be initialized */
    struct aws_linked_list list;

    /* Allocate a second list to check frame conditions (must remain unchanged) */
    struct aws_linked_list other;

    /* Nondeterministically initialize both structures */
    __CPROVER_assume(__CPROVER_is_fresh(&list, sizeof(list)));
    __CPROVER_assume(__CPROVER_is_fresh(&other, sizeof(other)));

    /* Save a copy of the second list for later comparison */
    struct aws_linked_list other_before = other;

    /* Call the function under test */
    aws_linked_list_init(&list);

    /* 1. Return value / error code correctness (validity predicates) */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));

    /* 2. Output buffer length/capacity invariants
       (aws_linked_list_init does not expose length/capacity fields, so no explicit checks needed) */

    /* 3. Memory not modified beyond the function's contract (frame conditions) */
    assert(other.head.next == other_before.head.next);
    assert(other.head.prev == other_before.head.prev);
    assert(other.tail.next == other_before.tail.next);
    assert(other.tail.prev == other_before.tail.prev);

    return 0;
}
