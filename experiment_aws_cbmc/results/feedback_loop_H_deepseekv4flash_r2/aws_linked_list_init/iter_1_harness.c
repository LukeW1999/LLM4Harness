#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness() {
    /* Non-deterministically allocate a linked list with bounded number of nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* Ensure list is not empty so that 'after' can be an interior node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Choose 'after' as the first node in the list (valid and has a next pointer) */
    struct aws_linked_list_node *after = list.head.next;
    __CPROVER_assume(aws_linked_list_node_next_is_valid(after));
    __CPROVER_assume(after != &list.tail); /* tail->next is NULL, so skip */

    /* Create a new node to insert. It must be non-NULL; previous values are overwritten */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Call the function */
    aws_linked_list_insert_after(after, to_add);

    /* Postcondition assertions from the function's specification */
    assert(after->next == to_add);
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(to_add));
    assert(aws_linked_list_node_next_is_valid(to_add));

    /* The list as a whole remains valid after insertion */
    assert(aws_linked_list_is_valid(&list));

    /* Unchanged fields: the list's head and tail nodes should still be properly linked */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    /* The head's next points to the first element; tail's prev to the last */
    /* No other fields to assert because the list has only head/tail nodes */
}
