#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* Non-deterministic data structures */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Initialize list: allocate and ensure validity */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Node is distinct from list internal nodes */
    __CPROVER_assume(&node != &list.head);
    __CPROVER_assume(&node != &list.tailapse);

    /* Save old state for immutability checks */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* Postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_tail_prev);

    /* Check head.next update for empty list case */
    if (old_tail_prev == &list.head) {
        assert(list.head.next == &node);
    } else {
        assert(old_tail_prev->next == &node);
    }
}
