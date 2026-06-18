#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create an existing node and add it to the list so that the list is non‑empty */
    struct aws_linked_list_node *existing = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(existing != NULL);
    /* the node is freshly allocated; its fields are nondet but will be set by push_back */
    aws_linked_list_push_back(&list, existing);

    /* 3. Choose the insertion point: before the tail sentinel (i.e., at the end) */
    struct aws_linked_list_node *before = (struct aws_linked_list_node *)&list.tail;

    /* 4. Allocate the node to be inserted and ensure it is not linked */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;

    /* 5. Save old state needed for post‑condition checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_last = list.tail.prev; /* node that was previously last */

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑conditions: the new node is correctly linked */
    assert(to_add->next == before);                 /* new node points forward to 'before' */
    assert(to_add->prev == old_last);               /* new node points back to previous last */
    assert(before->prev == to_add);                 /* 'before' now points back to new node */
    assert(list.tail.prev == to_add);               /* tail sentinel's prev is the new node */
    assert(old_last->next == to_add);               /* previous last now points forward to new node */

    /* 8. Unchanged fields (except those explicitly modified) */
    assert(list.head.prev == old_list.head.prev);   /* head.prev unchanged (should be NULL) */
    assert(list.head.next == old_list.head.next);   /* first element unchanged */
    assert(list.tail.next == old_list.tail.next);   /* tail.next unchanged (should be NULL) */
    assert(old_last->prev == &list.head);           /* previous last's prev still points to head */
    assert(existing->prev == &list.head);           /* existing node's prev unchanged */
    /* existing->next changed to point to the new node, which is expected, so we do not assert it unchanged */

    /* 9. The inserted node is now considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 10. Validity invariants must hold after the operation */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
