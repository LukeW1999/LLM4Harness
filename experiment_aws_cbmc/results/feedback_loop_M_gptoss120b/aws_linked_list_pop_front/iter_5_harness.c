#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;

    /* Allocate and bound the linked list */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The function requires a non‑empty list */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Ensure the list has at least two elements so it cannot become empty */
    struct aws_linked_list_node *first = list.head.next;
    __CPROVER_assume(first != &list.tail);
    struct aws_linked_list_node *second = first->next;
    __CPROVER_assume(second != &list.tail);

    /* Save a copy for immutability checks */
    struct aws_linked_list old = list;

    /* Call the function under test */
    aws_linked_list_pop_front(&list);

    /* Post‑conditions */
    assert(aws_linked_list_is_valid(&list));

    /* After removal, the new first node should be the former second node */
    assert(list.head.next == second);

    /* Tail's prev should stay the same (list cannot become empty because we assumed at least two elements) */
    assert(list.tail.prev == old.tail.prev);
}
