#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness(void) {
    struct aws_linked_list list;

    /* Initialize the list to a known state */
    aws_linked_list_init(&list);

    /* Assume the list satisfies the structural validity predicate */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Preserve the original state for frame condition checks */
    struct aws_linked_list old = list;

    /* Call the function under verification */
    const struct aws_linked_list_node *end = aws_linked_list_end(&list);

    /* Postcondition 1: Return value correctness */
    assert(end == &list.tail);

    /* Postcondition 2: The list remains valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 3: Frame condition – the list structure is unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    return 0;
}
