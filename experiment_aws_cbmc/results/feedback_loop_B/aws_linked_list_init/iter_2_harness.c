#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;

    /* 2. Initialize the linked list */
    aws_linked_list_init(&list);

    /* 3. Assert postconditions */
    assert(list.head.next == &list.tail);  // head.next points to tail
    assert(list.head.prev == NULL);        // head.prev is NULL
    assert(list.tail.next == NULL);        // tail.next is NULL
    assert(list.tail.prev == &list.head);  // tail.prev points to head

    /* 4. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
