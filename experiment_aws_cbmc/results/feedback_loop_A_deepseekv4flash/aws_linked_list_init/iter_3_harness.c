#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_init_harness() {
    struct aws_linked_list list;

    aws_linked_list_init(&list);

    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}

int main() {
    aws_linked_list_init_harness();
    return 0;
}
