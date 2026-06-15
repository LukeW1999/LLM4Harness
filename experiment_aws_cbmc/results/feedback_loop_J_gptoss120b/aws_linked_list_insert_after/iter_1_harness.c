#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is already in the list to insert after.
       For simplicity we use the head sentinel, which is always part of the list. */
    struct aws_linked_list_node *after = &list.head;

    /* 3. Allocate a fresh node to add */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    /* Ensure the node is not linked anywhere */
    to_add->next = NULL;
    to_add->prev = NULL;

    /* 4. Save old state before the call */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Post‑condition: linkage updates */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(old_after_next->prev == to_add);

    /* 7. Unchanged fields of the list structure */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(after->prev == old_after_prev);

    /* 8. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
