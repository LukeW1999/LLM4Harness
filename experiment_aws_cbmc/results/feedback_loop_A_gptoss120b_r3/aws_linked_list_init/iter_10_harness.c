#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;

    aws_linked_list_init(&list);

    /* Verify the list is correctly initialized */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(aws_linked_list_is_valid(&list));
}
