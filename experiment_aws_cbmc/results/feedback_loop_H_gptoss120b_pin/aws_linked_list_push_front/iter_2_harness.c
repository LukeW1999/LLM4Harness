#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    node.next = NULL;
    node.prev = NULL;

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    aws_linked_list_push_back(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_last);
    assert(list.tail.next == NULL);
    assert(list.head.prev == NULL);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    if (old.head.next == &old.tail) {
        assert(list.head.next == &node);
    } else {
        assert(list.head.next == old.head.next);
    }

    if (old_last != &old.tail) {
        assert(old_last->next == &node);
    }
}
