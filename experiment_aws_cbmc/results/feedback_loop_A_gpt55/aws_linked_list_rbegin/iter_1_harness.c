#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(result == old.tail.prev);

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    if (aws_linked_list_empty(&list)) {
        assert(result == &list.head);
        assert(result == aws_linked_list_rend(&list));
    } else {
        assert(result != aws_linked_list_end(&list));
        assert(result->next == &list.tail);
        assert(aws_linked_list_node_next_is_valid(result));
    }

    assert(aws_linked_list_is_valid(&list));
}
