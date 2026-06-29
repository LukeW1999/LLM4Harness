#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_init_harness(void) {
    /* Allocate a linked list on the stack */
    struct aws_linked_list list;

    /* Call the function under test */
    aws_linked_list_init(&list);

    /* Postcondition 1: The list is valid after initialization */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 2: The list is empty after initialization */
    assert(aws_linked_list_empty(&list));

    /* Postcondition 3: head.next points to tail */
    assert(list.head.next == &list.tail);

    /* Postcondition 4: head.prev is NULL */
    assert(list.head.prev == NULL);

    /* Postcondition 5: tail.prev points to head */
    assert(list.tail.prev == &list.head);

    /* Postcondition 6: tail.next is NULL */
    assert(list.tail.next == NULL);

    /* Postcondition 7: The head->next->prev == head (bidirectional link) */
    assert(list.head.next->prev == &list.head);

    /* Postcondition 8: The tail->prev->next == tail (bidirectional link) */
    assert(list.tail.prev->next == &list.tail);

    /* Postcondition 9: Deep validity check - head reaches tail */
    assert(aws_linked_list_is_valid_deep(&list));

    /* Postcondition 10: begin() returns tail (empty list) */
    assert(aws_linked_list_begin(&list) == &list.tail);

    /* Postcondition 11: end() returns tail */
    assert(aws_linked_list_end(&list) == &list.tail);

    /* Postcondition 12: rbegin() returns head (empty list) */
    assert(aws_linked_list_rbegin(&list) == &list.head);

    /* Postcondition 13: rend() returns head */
    assert(aws_linked_list_rend(&list) == &list.head);
}

void aws_linked_list_init_harness(void) {
    aws_linked_list_init_harness();
    return 0;
}
