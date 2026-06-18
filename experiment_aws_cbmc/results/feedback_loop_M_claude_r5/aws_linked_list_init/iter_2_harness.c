#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_init_harness() {
    /* Declare a linked list on the stack */
    struct aws_linked_list list;

    /* Call function under test */
    aws_linked_list_init(&list);

    /* Assert postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
