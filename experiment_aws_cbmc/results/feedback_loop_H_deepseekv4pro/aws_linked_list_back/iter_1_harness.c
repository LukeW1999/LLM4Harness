#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#    define MAX_LINKED_LIST_ITEM_ALLOCATION 10
#endif

void aws_linked_list_back_harness(void) {
    /* Initialize a valid, bounded linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save original state for immutability checks */
    struct aws_linked_list old = list;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* Postcondition 1: result points to the node that is tail's predecessor */
    assert(result == old.tail.prev);

    /* Postcondition 2: the list itself is not modified */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Postcondition 3: the list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 4: the returned node's next pointer is the tail (always true for a valid list) */
    assert(result->next == &list.tail);

    /* Postcondition 5: result is non‑NULL (it always points to a node inside the list) */
    assert(result != NULL);
}
