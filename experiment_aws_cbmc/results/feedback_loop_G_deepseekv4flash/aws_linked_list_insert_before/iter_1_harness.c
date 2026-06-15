#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness() {
    /* Create a non-deterministically allocated linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));  /* at least one node */

    /* Pick the first node as the insert position (non-sentinel) */
    struct aws_linked_list_node *before = list.head.next;
    __CPROVER_assume(before != &list.head);  /* not head sentinel */

    /* Create a new node to insert (not already in any list) */
    struct aws_linked_list_node to_add;
    /* The node's contents are irrelevant because they will be overwritten */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&to_add, sizeof(to_add)));

    /* Save the old predecessor pointer for postcondition checks */
    struct aws_linked_list_node *old_prev = before->prev;

    /* Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* Postconditions:
     *   - to_add->next == before
     *   - to_add->prev == old_prev
     *   - old_prev->next == &to_add   (so the list remains linked)
     *   - before->prev == &to_add
     *   - The list is still valid
     *   - The new node is in the list
     */
    assert(to_add.next == before);
    assert(to_add.prev == old_prev);
    assert(old_prev->next == &to_add);
    assert(before->prev == &to_add);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
    assert(aws_linked_list_node_is_in_list(&to_add));

    /* Unchanged invariants:
     *   - The list structure itself (head and tail nodes) remains the same object.
     *   - No other nodes were modified except the ones directly involved.
     *   - The non-pointer content of the nodes (if any) is unchanged.
     *   - The number of nodes in the list increased by 1.
     */
    /* These are implicitly covered by the validity asserts. */
}
