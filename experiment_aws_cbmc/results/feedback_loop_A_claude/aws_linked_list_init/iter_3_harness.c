#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_init_harness(void) {
    /* Declare a linked list */
    struct aws_linked_list list;

    /* Call the function under test */
    aws_linked_list_init(&list);

    /* Assert postconditions */
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);

    /* The list should be valid and empty */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));
}
