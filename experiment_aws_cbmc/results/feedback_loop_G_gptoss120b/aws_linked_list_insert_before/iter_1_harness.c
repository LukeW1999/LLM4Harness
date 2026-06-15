#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

/* Harness for aws_linked_list_insert_before */
void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Manually build a minimal non‑empty list (head <-> existing <-> tail) */
    struct aws_linked_list_node existing;
    /* sentinel connections */
    list.head.prev = &list.tail;
    list.tail.next = &list.head;
    /* existing node links */
    existing.prev = &list.head;
    existing.next = &list.tail;
    list.head.next = &existing;
    list.tail.prev = &existing;

    /* 3. Choose the 'before' node (must be in the list) */
    struct aws_linked_list_node *before = &existing;

    /* 4. Allocate the node to be inserted and ensure it is not linked */
    struct aws_linked_list_node to_add;
    to_add.next = NULL;
    to_add.prev = NULL;
    struct aws_linked_list_node *to_add_ptr = &to_add;

    /* 5. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add_ptr);

    /* ---------------------------------------------------------------------- */
    /* 7. Post‑condition checks                                               */
    /* ---------------------------------------------------------------------- */

    /* Changed fields (as per the specification of insert_before) */
    assert(before->prev == to_add_ptr);                     /* before now points back to the new node */
    assert(to_add_ptr->next == before);                    /* new node points forward to before */
    assert(to_add_ptr->prev == old_before_prev);           /* new node links back to old predecessor */
    assert(old_before_prev->next == to_add_ptr);           /* old predecessor now points forward to new node */

    /* Unchanged fields (explicitly asserted) */
    assert(before->next == old_before_next);               /* forward link of 'before' unchanged */
    /* The sentinel nodes themselves stay the same objects */
    assert(&list.head == &list.head);
    assert(&list.tail == &list.tail);
    /* The node that was previously after 'before' is still the same */
    assert(before->next == old_before_next);

    /* ---------------------------------------------------------------------- */
    /* 8. Validity invariants                                                 */
    /* ---------------------------------------------------------------------- */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
    assert(aws_linked_list_node_is_in_list(to_add_ptr));
}
