#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Ensure the list is non‑empty so that we have a valid 'before' node */
    struct aws_linked_list_node *existing = malloc(sizeof(*existing));
    __CPROVER_assume(existing != NULL);
    aws_linked_list_node_reset(existing);
    aws_linked_list_push_back(&list, existing);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Choose the node that will serve as 'before' */
    struct aws_linked_list_node *before = existing;
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before != &list.head);
    __CPROVER_assume(before != &list.tail);

    /* 4. Allocate the node to be inserted and ensure it is not linked */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL);
    __CPROVER_assume(to_add->prev == NULL);

    /* 5. Save the predecessor of 'before' for post‑condition checks */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑condition: linkage of the newly inserted node */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    assert(old_before_prev->next == to_add);

    /* 8. The inserted node must now be reported as being in a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 9. No other part of the list should be corrupted */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
