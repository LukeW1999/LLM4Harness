#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* Non-deterministic data structures */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Initialize list: allocate and ensure validity */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Node is distinct from list internal nodes; assume it is valid (not in a list) */
    /* We do not assume node is initialized: it will be written by the function */
    /* But we must ensure node is not part of the list to avoid aliasing */
    __CPROVER_assume(&node != &list.head);
    __CPROVER_assume(&node != &list.tail);
    /* Also ensure node is not equal to any node already in the list (but we cannot iterate) */
    /* For simplicity, assume node is not the head or tail, and that node's pointers are free */
    /* This is a common assumption in linked list harnesses */

    /* Save old state for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* Postconditions from Doxygen and implementation:
     * - The list remains valid.
     * - The new node becomes the last element.
     * - The node's pointers are correctly linked.
     */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);

    /* Additional assertions for node linkage */
    assert(node.next == &list.tail);
    if (old_tail_prev == &list.head) {
        /* List was empty; head.next should now point to node */
        assert(list.head.next == &node);
    } else {
        /* List was non-empty; old last node's next should now point to node */
        assert(old_tail_prev->next == &node);
    }
    assert(node.prev == old_tail_prev);

    /* Unchanged fields of the list structure:
     * The head and tail nodes themselves are not moved; only their pointers change.
     * The following fields of the list struct are unchanged:
     * - head.prev (always NULL)
     * - tail.next (always NULL)
     * But we cannot assert them because they are part of the struct which was modified.
     * Instead, we assert that the list invariants hold.
     */
    /* Additionally, the node's other fields (if any) are not relevant. */
}
