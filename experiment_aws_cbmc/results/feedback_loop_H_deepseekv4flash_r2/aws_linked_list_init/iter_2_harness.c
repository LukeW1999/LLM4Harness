#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_init_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(list.head.prev == NULL);
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
}
