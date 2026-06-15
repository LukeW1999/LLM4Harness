#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    // Use the head sentinel as the node after which to insert.
    struct aws_linked_list_node *after = &list.head;
    __CPROVER_assume(after->next != NULL); // Head always has a next (tail or data node)

    // Save the old next pointer of 'after'
    struct aws_linked_list_node *old_next = after->next;

    // Allocate a new node to insert
    struct aws_linked_list_node to_add;
    // The new node's pointers will be overwritten; no need to initialize

    // Call the function under test
    aws_linked_list_insert_after(after, &to_add);

    // 1. The list remains valid
    assert(aws_linked_list_is_valid_deep(&list));

    // 2. Specific pointer updates for the inserted node and its neighbors
    assert(after->next == &to_add);
    assert(to_add->prev == after);
    assert(to_add->next == old_next);
    assert(old_next->prev == &to_add);

    // 3. Unchanged fields: The head and tail nodes themselves are the same objects.
    //    Their prev/next pointers that were not updated remain as before.
    //    For head: head.prev is unchanged (always NULL in a valid empty list? Actually
    //    in aws linked list, head.prev is not used and should remain unchanged).
    //    Similarly, tail.next is unchanged. We check that these are still NULL.
    //    (This is part of validity, but we explicitly assert to satisfy the pattern.)
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    // 4. Validity invariants hold
    assert(aws_linked_list_is_valid(&list));
}
