#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Initialize the list to a known empty state */
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a node that will already be in the list (after) */
    struct aws_linked_list_node after_node;
    /* push_back will set the proper links for after_node */
    aws_linked_list_push_back(&list, &after_node);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Create a node to be inserted (to_add) and ensure it is not in any list */
    struct aws_linked_list_node to_add;
    aws_linked_list_node_reset(&to_add); /* sets next and prev to NULL */

    /* 4. Save old state for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_after_next = after_node.next;
    struct aws_linked_list_node *old_after_prev = after_node.prev;
    struct aws_linked_list_node *old_next_next = NULL;
    if (old_after_next != NULL) {
        old_next_next = old_after_next->next;
    }

    /* 5. Call the function under test */
    aws_linked_list_insert_after(&after_node, &to_add);

    /* 6. Post‑condition assertions */

    /* Links that must have changed */
    assert(after_node.next == &to_add);
    assert(to_add.prev == &after_node);
    assert(to_add.next == old_after_next);
    if (old_after_next != NULL) {
        assert(old_after_next->prev == &to_add);
    }

    /* The list head and tail sentinel nodes must remain unchanged */
    assert(list.head.prev == old_list.head.prev);
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == (old_after_next == &list.tail ? &to_add : old_list.tail.prev));
    assert(list.tail.next == old_list.tail.next);

    /* The original 'after' node's prev must stay the same */
    assert(after_node.prev == old_after_prev);

    /* The node that followed 'after' (if any) must keep its next pointer */
    if (old_after_next != NULL) {
        assert(old_after_next->next == old_next_next);
    }

    /* 7. Validity invariant must hold after the operation */
    assert(aws_linked_list_is_valid(&list));
}
