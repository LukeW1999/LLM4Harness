#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_rend_harness(void) {
    /* Allocate a linked list */
    struct aws_linked_list list;

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save state before function call for frame condition checks */
    struct aws_linked_list_node old_head = list.head;
    struct aws_linked_list_node old_tail = list.tail;

    /* Call the function under test */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* Postcondition 1: Return value correctness
     * rend should point to the head of the list (one before the first element) */
    assert(rend == &list.head);

    /* Postcondition 2: Return value is non-null */
    assert(rend != NULL);

    /* Postcondition 3: Frame condition - list head is not modified */
    assert(list.head.next == old_head.next);
    assert(list.head.prev == old_head.prev);

    /* Postcondition 4: Frame condition - list tail is not modified */
    assert(list.tail.next == old_tail.next);
    assert(list.tail.prev == old_tail.prev);

    /* Postcondition 5: List validity is preserved */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 6: The returned pointer is the address of list.head */
    assert(rend == (const struct aws_linked_list_node *)&list.head);
}
