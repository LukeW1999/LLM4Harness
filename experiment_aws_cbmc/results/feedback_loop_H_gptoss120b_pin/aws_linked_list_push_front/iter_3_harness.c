#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    node.next = NULL;
    node.prev = NULL;

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_first = list.head.next;

    aws_linked_list_push_front(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == NULL);
    assert(list.head.prev == NULL);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    if (old_last != &old.tail) {
        assert(old_last->next == &node);
    }
}
