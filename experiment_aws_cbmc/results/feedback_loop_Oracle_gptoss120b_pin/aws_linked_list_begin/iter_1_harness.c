#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_begin_harness(void) {
    struct aws_linked_list list;

    /* Assume the list is structurally valid. */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Snapshot the list structure for frame condition checks. */
    struct aws_linked_list old_list = list;

    /* Call the function under verification. */
    struct aws_linked_list_node *rval = aws_linked_list_begin(&list);

    /* Postcondition: return value correctness. */
    if (aws_linked_list_empty(&list)) {
        assert(rval == &list.tail);
    } else {
        assert(rval != &list.tail);
        assert(rval->prev == &list.head);
        assert(aws_linked_list_node_next_is_valid(rval));
        assert(aws_linked_list_node_prev_is_valid(rval));
    }

    /* Frame condition: the list structure must not be modified. */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    return 0;
}
