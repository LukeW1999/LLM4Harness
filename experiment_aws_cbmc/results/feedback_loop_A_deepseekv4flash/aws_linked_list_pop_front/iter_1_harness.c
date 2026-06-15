#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    /* 1. Non-deterministically allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Non-deterministically create a node to insert */
    struct aws_linked_list_node node;
    struct aws_linked_list_node *old_head = list.head.next;

    /* 3. Save old state BEFORE calling */
    struct aws_linked_list old = list;

    /* 4. Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* 5. Assert postconditions */
    /* 5.a. Changed fields */
    assert(list.head.next == &node);  // node becomes new first element
    assert(node.next == old_head);    // node points to old head
    assert(node.prev == &list.head);  // node prev points to list head
    if (!aws_linked_list_empty(&old)) {
        assert(old_head->prev == &node);  // old head now points back to node
    }

    /* 5.b. Unchanged fields: tail pointers must remain the same */
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    /* head.prev is always NULL (invariant from init) */
    assert(list.head.prev == NULL);

    /* 5.c. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
