#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Initialise an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Populate the list with a nondet number of nodes (bounded) */
    make_linked_list(&list, MAX_LINKED_LIST_ITEM_ALLOCATION, aws_default_allocator());
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Choose a node that is in the list to serve as 'before'.
       It must not be the head sentinel (inserting before head is undefined). */
    struct aws_linked_list_node *before;
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(before));
    __CPROVER_assume(before != &list.head);

    /* 4. Allocate a fresh node to insert (not currently in any list) */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 5. Save neighbours of 'before' for post‑condition checks */
    struct aws_linked_list_node *old_prev = before->prev;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑condition checks */
    assert(to_add->next == before);                 /* new node points forward to 'before' */
    assert(to_add->prev == old_prev);               /* new node points back to previous node */
    assert(before->prev == to_add);                 /* 'before' now points back to new node */
    assert(old_prev->next == to_add);               /* previous node now points forward to new node */

    /* 8. Unchanged parts of the list */
    assert(list.head.next != NULL);                 /* head still points to first real node or tail */
    assert(list.tail.prev != NULL);                 /* tail still points to last real node */

    /* 9. List invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_next_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(to_add));
}
