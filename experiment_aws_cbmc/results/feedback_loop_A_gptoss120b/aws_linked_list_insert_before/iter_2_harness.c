#include <assert.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and initialise a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a first real node and add it to the list */
    struct aws_linked_list_node *first = malloc(sizeof(*first));
    __CPROVER_assume(first != NULL);
    aws_linked_list_node_reset(first);
    aws_linked_list_push_back(&list, first);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Choose a node to insert before: either the tail sentinel or the first element */
    struct aws_linked_list_node *before;
    if (nondet_bool()) {
        before = &list.tail;          /* insert before tail sentinel (push_back case) */
    } else {
        before = list.head.next;      /* insert before the first real element */
    }

    /* 4. Allocate the node that will be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);   /* next == prev == NULL */

    /* 5. Save old state needed for post‑condition checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑conditions */

    /* The newly added node is linked correctly */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(aws_linked_list_node_is_in_list(to_add));

    /* The “before” node now points back to the new node */
    assert(before->prev == to_add);

    /* The node that previously preceded “before” now points forward to the new node,
       unless it was the head sentinel. */
    if (old_before_prev != &list.head) {
        assert(old_before_prev->next == to_add);
    }

    /* List validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Fields of the list structure that must never change */
    assert(list.head.prev == old_list.head.prev);   /* should still be &list.head */
    assert(list.tail.next == old_list.tail.next);   /* should still be &list.tail */
}
