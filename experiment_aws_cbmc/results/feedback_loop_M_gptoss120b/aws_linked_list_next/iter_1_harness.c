#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* CBMC harness for aws_linked_list_next */
void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is part of the list.
       For simplicity we use the first real node (which may be the tail sentinel if the list is empty). */
    const struct aws_linked_list_node *node = list.head.next;

    /* 3. Save old state */
    struct aws_linked_list old = list;
    const struct aws_linked_list_node *old_node = node;
    const struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Post‑condition: the returned pointer is exactly the original next pointer */
    assert(result == old_next);

    /* 6. Unchanged fields of the list structure */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 7. Unchanged fields of the examined node */
    assert(node->next == old_next);
    assert(node->prev == old_node->prev);

    /* 8. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
