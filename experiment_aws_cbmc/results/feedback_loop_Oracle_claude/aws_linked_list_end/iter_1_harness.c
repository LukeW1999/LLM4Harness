#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_end_harness(void) {
    struct aws_linked_list list;

    /* Nondeterministically initialize the linked list nodes */
    /* Set up head and tail to form a valid empty or non-empty list */
    list.head.prev = NULL;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;
    list.tail.next = NULL;

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Call the function under test */
    const struct aws_linked_list_node *end = aws_linked_list_end(&list);

    /* Postcondition 1: Return value must not be NULL */
    assert(end != NULL);

    /* Postcondition 2: Return value must point to the tail of the list */
    assert(end == &list.tail);

    /* Postcondition 3: The list must still be valid after the call (frame condition) */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 4: The tail's next pointer is still NULL (structure not modified) */
    assert(list.tail.next == NULL);

    /* Postcondition 5: The head's prev pointer is still NULL (structure not modified) */
    assert(list.head.prev == NULL);

    /* Postcondition 6: The head's next still points to tail (structure not modified) */
    assert(list.head.next == &list.tail);

    /* Postcondition 7: The tail's prev still points to head (structure not modified) */
    assert(list.tail.prev == &list.head);
}

void aws_linked_list_end_harness(void) {
    aws_linked_list_end_harness();
    return 0;
}
