#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev; /* previous last element */

    /* 3. Allocate a node to insert */
    struct aws_linked_list_node node; /* non‑deterministic contents are fine */

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* 5. Postconditions on success (function is void, so only one path) */
    /* The new node becomes the last element */
    assert(list.tail.prev == &node);
    /* The new node points to the tail sentinel */
    assert(node.next == &list.tail);
    /* The new node links back to the previous last element */
    assert(node.prev == old_last);
    /* The previous last element now points forward to the new node */
    assert(old_last->next == &node);

    /* 6. Unchanged fields of the sentinel nodes */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* 7. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
