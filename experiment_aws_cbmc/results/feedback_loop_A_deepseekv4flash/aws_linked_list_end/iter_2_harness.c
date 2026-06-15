#include <aws/common/linked_list.h>

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    list.head.prev = NULL;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;
    list.tail.next = NULL;

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *end = aws_linked_list_end(&list);

    assert(end == &list.tail);
    assert(end != NULL);

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    assert(aws_linked_list_is_valid(&list));
}
