#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is in the list to serve as 'before'.
       For simplicity we use the list tail sentinel, which is always
       part of a valid list. */
    struct aws_linked_list_node *before = &list.tail;

    /* 3. Allocate a fresh node to insert */
    struct aws_linked_list_node to_add;
    /* The node must not be in any list before insertion */
    to_add.next = NULL;
    to_add.prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    /* 4. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_last = list.tail.prev;   /* node that was last before insertion */
    struct aws_linked_list_node old_head_next = list.head.next; /* first real node (or tail if empty) */

    /* 5. Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* 6. Post‑condition: the new node is correctly linked */
    assert(to_add.next == before);                /* new node points forward to 'before' */
    assert(to_add.prev == old_last);              /* new node points back to previous last */
    assert(before->prev == &to_add);              /* 'before' now points back to new node */
    assert(old_last->next == &to_add);            /* previous last now points forward to new node */

    /* 7. Unchanged parts of the list */
    assert(list.head.next == old_head_next);      /* head's next unchanged */
    assert(list.tail.prev == &to_add);            /* tail's prev is the new node (expected change) */
    /* All other nodes' next/prev remain unchanged – this is implicitly covered by the
       above assertions because we only modified the connections involving the
       inserted node and its immediate neighbours. */

    /* 8. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_next_is_valid(&to_add) == false); /* next is tail sentinel */
    assert(aws_linked_list_node_prev_is_valid(&to_add));
}
