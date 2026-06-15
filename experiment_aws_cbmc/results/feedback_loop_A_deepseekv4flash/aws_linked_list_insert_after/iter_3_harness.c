#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node interior;
    struct aws_linked_list_node to_add;

    /* initialize head sentinel */
    list.head.prev = &list.head;
    list.head.next = &interior;

    /* initialize tail sentinel */
    list.tail.prev = &interior;
    list.tail.next = &list.tail;

    /* initialize the interior node to link head and tail */
    interior.prev = &list.head;
    interior.next = &list.tail;

    /* The node to insert is initially isolated */
    to_add.prev = NULL;
    to_add.next = NULL;

    /* ensure the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* call the function under test */
    aws_linked_list_insert_after(&interior, &to_add);

    /* Check postconditions */
    assert(interior.next == &to_add);
    assert(to_add.prev == &interior);
    assert(to_add.next == &list.tail);
    assert(list.head.next == &interior);
    assert(list.head.prev == &list.head);
    assert(list.tail.prev == &to_add);
    assert(list.tail.next == &list.tail);
    assert(aws_linked_list_is_valid_deep(&list));
}
