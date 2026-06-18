#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);

    /* 2. Assume the list is initially valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 3. Choose the node after which to insert.
       Use the list head sentinel (always present and not the tail). */
    struct aws_linked_list_node *after = &list.head;

    /* 4. Allocate a node to add and assume it is not already in a list */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 5. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_head_prev  = list.head.prev;
    struct aws_linked_list_node *old_tail_next  = list.tail.next;
    struct aws_linked_list_node *old_tail_prev  = list.tail.prev;

    struct aws_linked_list old_list = list; /* shallow copy for unchanged fields */

    /* 6. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 7. Post‑condition: pointers updated correctly */
    assert(after->next == to_add);                     /* after now points to new node */
    assert(to_add->prev == after);                     /* new node points back to after */
    assert(to_add->next == old_after_next);           /* new node links to original successor */
    assert(old_after_next->prev == to_add);            /* original successor now points back to new node */

    /* 8. List sentinel updates (head/tail) */
    assert(list.head.next == to_add);                 /* head's next is the new first element */
    assert(list.tail.prev == to_add);                 /* tail's prev is the new last element */

    /* 9. Unchanged fields */
    assert(after->prev == old_after_prev);            /* after's prev unchanged */
    assert(list.head.prev == old_head_prev);          /* head.prev unchanged */
    assert(list.tail.next == old_tail_next);           /* tail.next unchanged */
    assert(list.tail.prev == to_add);                 /* tail.prev changed as expected, already asserted */
    assert(list.head.next == to_add);                 /* head.next changed as expected, already asserted */

    /* 10. The rest of the list (if any) must remain unchanged */
    /* Since we started with an empty list, there are no other nodes to check.
       For a non‑empty list, the saved pointers above guarantee that only the
       immediate neighbours of the insertion point are modified. */

    /* 11. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
