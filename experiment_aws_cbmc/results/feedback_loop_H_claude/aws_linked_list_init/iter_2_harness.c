#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_init_harness() {
    /* 1. Declare the linked list on the stack */
    struct aws_linked_list list;

    /* 2. Call function under test */
    aws_linked_list_init(&list);

    /* 3. Assert postconditions */

    /* The list must be valid after init */
    assert(aws_linked_list_is_valid(&list));

    /* The list must be empty after init */
    assert(aws_linked_list_empty(&list));

    /* head.next points to tail */
    assert(list.head.next == &list.tail);

    /* tail.prev points to head */
    assert(list.tail.prev == &list.head);

    /* head.prev is NULL */
    assert(list.head.prev == NULL);

    /* tail.next is NULL */
    assert(list.tail.next == NULL);
}
