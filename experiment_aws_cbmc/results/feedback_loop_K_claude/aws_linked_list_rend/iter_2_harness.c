#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_rend_harness(void) {
    /* Stack-allocate a linked list */
    struct aws_linked_list list;

    /* Initialize the list to ensure valid structure */
    aws_linked_list_init(&list);

    /* Save state before call to verify frame conditions */
    struct aws_linked_list_node *head_next_before = list.head.next;
    struct aws_linked_list_node *head_prev_before = list.head.prev;
    struct aws_linked_list_node *tail_next_before = list.tail.next;
    struct aws_linked_list_node *tail_prev_before = list.tail.prev;

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Call the function under test */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* Postcondition: return value is non-NULL */
    assert(rend != NULL);

    /* Postcondition: return value points to head sentinel */
    assert(rend == &list.head);

    /* Postcondition: frame - list structure is not modified */
    assert(list.head.next == head_next_before);
    assert(list.head.prev == head_prev_before);
    assert(list.tail.next == tail_next_before);
    assert(list.tail.prev == tail_prev_before);

    /* Postcondition: list remains valid after the call */
    assert(aws_linked_list_is_valid(&list));
}
