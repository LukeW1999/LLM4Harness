#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node that will be in the list (after) */
    struct aws_linked_list_node *after = malloc(sizeof(*after));
    __CPROVER_assume(after != NULL);
    aws_linked_list_node_reset(after);
    /* Insert the node into the list so that it becomes a valid list element */
    aws_linked_list_push_back(&list, after);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Allocate a node that will be added (to_add) and ensure it is not in any list */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 4. Save old state before the call */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_after = *after;
    struct aws_linked_list_node *old_after_next = after->next;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Post‑condition: pointers must be linked correctly */
    assert(after->next == to_add);                     /* after now points to to_add */
    assert(to_add->prev == after);                     /* to_add points back to after */
    assert(to_add->next == old_after_next);            /* to_add links to the former next node */
    assert(old_after_next->prev == to_add);            /* former next node now points back to to_add */

    /* 7. Unchanged fields (head/tail sentinel links) */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    /* after's prev should be unchanged */
    assert(after->prev == old_after.prev);
    /* If the list was empty before insertion (after was the only element),
       tail.prev should now be to_add; otherwise it remains unchanged. */
    if (old_after_next == &list.tail) {
        assert(list.tail.prev == to_add);
    } else {
        assert(list.tail.prev == old_list.tail.prev);
    }

    /* 8. Validity invariant must hold after the operation */
    assert(aws_linked_list_is_valid(&list));
}
