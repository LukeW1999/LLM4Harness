#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_front_harness(void) {
    struct aws_linked_list list;

    /* Allocate and bound the linked list */
    ensure_linked_list_is_allocated(&list, 5);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Node to be inserted */
    struct aws_linked_list_node node;
    node.prev = NULL;
    node.next = NULL;

    /* Save the original first element (may be the tail sentinel) */
    struct aws_linked_list_node *old_first = list.head.next;

    /* Call the function under test */
    aws_linked_list_push_front(&list, &node);

    /* Postconditions */
    assert(aws_linked_list_is_valid(&list));

    /* The new node must be the first element */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);

    if (old_first != &list.tail) {
        /* If the list was non‑empty, the previous first node now follows the new node */
        assert(old_first->prev == &node);
    } else {
        /* If the list was empty, the tail sentinel now follows the new node */
        assert(list.tail.prev == &node);
    }
}
