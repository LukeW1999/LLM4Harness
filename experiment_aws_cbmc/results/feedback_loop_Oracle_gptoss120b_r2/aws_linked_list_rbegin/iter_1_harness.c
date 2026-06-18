#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list list_old = list;

    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    /* Frame condition: the list structure must remain unchanged */
    assert(list.head.next == list_old.head.next);
    assert(list.head.prev == list_old.head.prev);
    assert(list.tail.next == list_old.tail.next);
    assert(list.tail.prev == list_old.tail.prev);

    /* Return value must be a valid iterator */
    if (aws_linked_list_empty(&list)) {
        /* Empty list: rbegin should point to the dummy head */
        assert(r == &list.head);
    } else {
        /* Non‑empty list: rbegin points to the last real node */
        assert(r != NULL);
        assert(r != &list.head);
        assert(r != &list.tail);
        assert(aws_linked_list_node_next_is_valid(r));
        assert(aws_linked_list_node_prev_is_valid(r));
        assert(aws_linked_list_node_is_in_list(r));
    }

    return 0;
}
