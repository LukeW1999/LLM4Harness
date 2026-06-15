#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness() {
    /* 1. Create and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Non-deterministically select a node in the list */
    struct aws_linked_list_node *node = aws_linked_list_begin(&list);
    size_t steps = nondet_size_t();
    __CPROVER_assume(steps < MAX_LINKED_LIST_ITEM_ALLOCATION);
    for (size_t i = 0; i < steps && node != aws_linked_list_end(&list); ++i) {
        node = aws_linked_list_next(node);
    }
    __CPROVER_assume(node != aws_linked_list_end(&list));

    /* 3. Save the old neighbors for checking connectivity */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Assert postconditions */

    /* Node's own pointers must be NULL */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* The former neighbors must now point to each other */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* The list invariant must hold after removal */
    assert(aws_linked_list_is_valid(&list));
}
