#include <aws/common/linked_list.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_rend_harness(void) {
    struct aws_linked_list list;

    /* nondeterministic initialization of the list structure */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy of the list to check for unintended modifications */
    struct aws_linked_list list_snapshot = list;

    /* Call the function under verification */
    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    /* Postcondition 1: return value correctness */
    assert(r == &list.head);

    /* Postcondition 2: (no length/capacity invariants for rend) */

    /* Postcondition 3: frame condition – the list must remain unchanged */
    assert(memcmp(&list, &list_snapshot, sizeof(list)) == 0);

    return 0;
}
