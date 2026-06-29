#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_init_harness(void) {
    /* Allocate a linked list on the heap (non-deterministic contents) */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));

    /* Precondition: list must be non-null */
    __CPROVER_assume(list != NULL);

    /* Call the function under test */
    aws_linked_list_init(list);

    /* Postcondition: head.next points to tail */
    assert(list->head.next == &list->tail);

    /* Postcondition: head.prev is NULL */
    assert(list->head.prev == NULL);

    /* Postcondition: tail.prev points to head */
    assert(list->tail.prev == &list->head);

    /* Postcondition: tail.next is NULL */
    assert(list->tail.next == NULL);

    /* Postcondition: list is valid per aws_linked_list_is_valid */
    assert(aws_linked_list_is_valid(list));

    /* Postcondition: list is empty per aws_linked_list_empty */
    assert(aws_linked_list_empty(list));
}

void aws_linked_list_init_harness(void) {
    aws_linked_list_init_harness();
    return 0;
}
