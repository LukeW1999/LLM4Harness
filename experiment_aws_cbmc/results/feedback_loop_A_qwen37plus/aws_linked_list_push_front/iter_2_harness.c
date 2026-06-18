#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old_list = list;
    bool was_empty = aws_linked_list_empty(&old_list);

    aws_linked_list_push_front(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_list.head.next);
    
    if (was_empty) {
        assert(list.tail.prev == &node);
        assert(node.next == &list.tail);
    } else {
        assert(old_list.head.next->prev == &node);
    }
}
