#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Use the head node as 'after' - it always has a valid next pointer */
    struct aws_linked_list_node *after = &list.head;

    /* Save the old next of 'after' before insertion */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* Create the node to insert */
    struct aws_linked_list_node to_add_node;
    struct aws_linked_list_node *to_add = &to_add_node;

    /* Preconditions */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL);
    __CPROVER_assume(to_add != NULL);

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postconditions: verify the linkage is correct */
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(after->next == to_add);
    assert(old_after_next->prev == to_add);
    assert(after->prev == old_after_prev);
}
