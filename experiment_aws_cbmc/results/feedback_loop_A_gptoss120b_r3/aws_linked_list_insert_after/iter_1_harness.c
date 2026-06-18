#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is guaranteed to be in the list.
       Using the head sentinel works for any list (including empty). */
    struct aws_linked_list_node *after = &list.head;
    __CPROVER_assume(aws_linked_list_node_next_is_valid(after));

    /* 3. Allocate a node that is not currently in any list */
    struct aws_linked_list_node to_add;
    to_add.prev = NULL;
    to_add.next = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    /* 4. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_to_add = to_add;
    struct aws_linked_list_node *old_next = after->next;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, &to_add);

    /* 6. Assert post‑conditions that must hold after a successful insertion */
    assert(to_add.prev == after);               /* new node points back to after */
    assert(to_add.next == old_next);            /* new node points forward to the old next */
    assert(after->next == &to_add);            /* after now points forward to new node */
    assert(old_next->prev == &to_add);          /* old next now points back to new node */

    /* 7. Assert fields that must remain unchanged */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 8. The list must still satisfy its validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
