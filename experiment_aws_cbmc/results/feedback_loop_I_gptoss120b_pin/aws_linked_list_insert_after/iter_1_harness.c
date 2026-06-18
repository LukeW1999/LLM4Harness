#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node that will be in the list (after) */
    struct aws_linked_list_node *after = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after != NULL);
    /* Insert the node into the list so that it is a valid list node */
    aws_linked_list_push_back(&list, after);
    __CPROVER_assume(aws_linked_list_node_is_in_list(after));

    /* 3. Allocate a node that is NOT in any list (to_add) */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 4. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list old_list = list;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Assert changed fields */
    assert(after->next == to_add);                     /* after now points to to_add */
    assert(to_add->prev == after);                     /* to_add's prev is after */
    assert(to_add->next == old_after_next);            /* to_add's next is the former after->next */
    assert(old_after_next->prev == to_add);            /* former next node now points back to to_add */

    /* 7. Assert unchanged fields */
    assert(after->prev == old_after_prev);             /* after's prev unchanged */
    assert(list.head.prev == old_list.head.prev);      /* list head's prev unchanged */
    assert(list.tail.next == old_list.tail.next);      /* list tail's next unchanged */

    /* 8. Assert that to_add is now part of the list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 9. Assert validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
