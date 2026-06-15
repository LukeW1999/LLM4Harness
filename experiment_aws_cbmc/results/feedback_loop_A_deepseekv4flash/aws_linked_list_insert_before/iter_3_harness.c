#include <aws/common/linked_list.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    /* Initialize the list with one actual node 'before' and sentinel head */
    list.head.next = &before;
    list.head.prev = &before;
    before.next = &list.head;
    before.prev = &list.head;

    /* Call the function */
    aws_linked_list_insert_before(&before, &to_add);

    /* Postconditions */
    assert(to_add.next == &before);
    assert(to_add.prev == &list.head);
    assert(before.prev == &to_add);
    assert(before.next == &list.head);
    assert(list.head.next == &to_add);
    assert(list.head.prev == &before);

    /* Ensure the list remains a valid circular doubly linked list (simple check) */
    assert(list.head.next->prev == &list.head);
    assert(list.head.prev->next == &list.head);
}
