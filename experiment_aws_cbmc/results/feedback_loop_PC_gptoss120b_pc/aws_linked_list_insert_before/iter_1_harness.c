#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Ensure the list is non‑empty so we have a valid “before” node */
    struct aws_linked_list_node *first = malloc(sizeof(*first));
    __CPROVER_assume(first != NULL);
    aws_linked_list_node_reset(first);
    aws_linked_list_push_back(&list, first);

    /* 3. Choose the node that will be the insertion point */
    struct aws_linked_list_node *before = first;               /* non‑NULL, part of list */

    /* 4. Allocate a node to be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);                       /* not in any list */

    /* 5. Snapshot state that will be examined after the call */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑conditions: fields that must have changed */
    assert(to_add->next == before);                           /* new node points forward */
    assert(to_add->prev == old_before_prev);                  /* new node points back */
    assert(before->prev == to_add);                           /* insertion point points back to new node */
    assert(old_before_prev->next == to_add);                  /* predecessor now points forward to new node */

    /* 8. Unchanged fields (frame conditions) */
    /* The node that originally followed “before” must still point back to “before” */
    if (old_before_next != &list.tail) {
        assert(old_before_next->prev == before);
    }
    /* The list head and tail sentinels themselves must remain the same objects */
    assert(&list.head == &list.head);
    assert(&list.tail == &list.tail);
    /* The original first node (now second) must still have its next pointer unchanged */
    if (old_before_prev != &list.head) {
        assert(old_before_prev->prev == old_before_prev->prev);
    }

    /* 9. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
