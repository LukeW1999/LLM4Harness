#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_front_harness(void) {
    /* Allocate the list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate one or two nodes non-deterministically to ensure the list
     * is non-empty (precondition) while keeping the state space bounded. */
    struct aws_linked_list_node node1;
    struct aws_linked_list_node node2;

    /* Always push at least one node so the precondition holds */
    aws_linked_list_push_back(&list, &node1);

    /* Non-deterministically add a second node */
    bool add_second;
    __CPROVER_assume(add_second == true || add_second == false);
    if (add_second) {
        aws_linked_list_push_back(&list, &node2);
    }

    /* Verify preconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));

    /* Remember whether the list had exactly one element */
    bool was_singleton = (list.head.next == &node1 && node1.next == &list.tail);

    /* Call the function under test */
    struct aws_linked_list_node *front = aws_linked_list_pop_front(&list);

    /* Postcondition: return value is non-null and equals node1 */
    assert(front != NULL);
    assert(front == &node1);

    /* Postcondition: returned node has been reset */
    assert(front->next == NULL);
    assert(front->prev == NULL);

    /* Postcondition: list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition: if the list was a singleton, it is now empty */
    if (was_singleton) {
        assert(aws_linked_list_empty(&list));
    }

    /* Postcondition: if a second node was added, it is now the front */
    if (add_second) {
        assert(!aws_linked_list_empty(&list));
        assert(list.head.next == &node2);
        assert(node2.prev == &list.head);
        assert(node2.next == &list.tail);
        assert(list.tail.prev == &node2);
    }
}
