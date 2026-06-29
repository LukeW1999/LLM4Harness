#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_end_harness(void) {
    /* Allocate a linked list */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Initialize the list to ensure structural validity */
    aws_linked_list_init(list);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Save state before call for frame condition checks */
    struct aws_linked_list_node *old_head_next = list->head.next;
    struct aws_linked_list_node *old_head_prev = list->head.prev;
    struct aws_linked_list_node *old_tail_next = list->tail.next;
    struct aws_linked_list_node *old_tail_prev = list->tail.prev;

    /* Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(list);

    /* Postcondition: return value is non-NULL */
    assert(result != NULL);

    /* Postcondition: return value points to list->tail */
    assert(result == &list->tail);

    /* Postcondition (frame): list structure is not modified */
    assert(list->head.next == old_head_next);
    assert(list->head.prev == old_head_prev);
    assert(list->tail.next == old_tail_next);
    assert(list->tail.prev == old_tail_prev);

    /* Postcondition: list remains valid after the call */
    assert(aws_linked_list_is_valid(list));
}

void aws_linked_list_end_harness(void) {
    aws_linked_list_end_harness();
    return 0;
}
