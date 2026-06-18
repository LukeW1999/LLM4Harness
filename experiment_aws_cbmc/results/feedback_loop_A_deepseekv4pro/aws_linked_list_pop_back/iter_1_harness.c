#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Declare the node to be added */
    struct aws_linked_list_node node;
    /* node is a local variable, its memory is always valid and writable.
     * No further assumptions required; the function writes to it anyway.
     */

    /* 2. Save old state, especially the tail’s predecessor */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_last = list.tail.prev; /* may be &list.head if empty */

    /* 3. Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* 4. Assert postconditions from the Doxygen / implementation */
    /* List remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* The new node is the last element */
    assert(list.tail.prev == &node);

    /* The node is correctly linked after the previous last element */
    assert(node.prev == old_last);
    assert(node.next == &list.tail);

    /* The previous last element now points to the new node */
    assert(old_last->next == &node);

    /* Unchanged sentinel fields: head.prev and tail.next remain NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* The head sentinel itself is unchanged (except head.next which may have been modified 
     * if the list was empty; we do not assert anything about head.next because that can change).
     */
    /* The tail sentinel’s address is the same as before (embedded in list) — always true */
}
