#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

/* Harness for aws_linked_list_insert_after */
void aws_linked_list_insert_after_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is already in the list to act as 'after'.
       For simplicity we use the head sentinel which is always present. */
    struct aws_linked_list_node *after = &list.head;

    /* 3. Allocate a node to be inserted and ensure it is not linked */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    to_add->prev = NULL;
    to_add->next = NULL;

    /* 4. Additional safety assumptions */
    __CPROVER_assume(after != to_add);
    __CPROVER_assume(after->next != NULL);               /* list must be well‑formed */
    __CPROVER_assume(after->next != to_add);            /* to_add is not already in the list */

    /* 5. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_after = *after;
    struct aws_linked_list_node *old_next_ptr = after->next;
    struct aws_linked_list_node old_next = *old_next_ptr;
    struct aws_linked_list_node old_to_add = *to_add;

    /* 6. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 7. Post‑condition: fields that must have changed */
    assert(to_add->prev == after);                     /* to_add.prev set to after */
    assert(to_add->next == old_next_ptr);              /* to_add.next set to former after->next */
    assert(after->next == to_add);                     /* after.next now points to to_add */
    assert(old_next_ptr->prev == to_add);              /* former next node's prev now points to to_add */

    /* 8. Post‑condition: fields that must remain unchanged */
    assert(after->prev == old_after.prev);             /* after.prev unchanged */
    assert(old_next.prev == old_next.prev);            /* (trivial, kept for completeness) */
    assert(old_next.next == old_next.next);            /* (trivial) */
    assert(list.head.prev == old_list.head.prev);      /* list head.prev unchanged */
    assert(list.tail.next == old_list.tail.next);      /* list tail.next unchanged */
    /* The tail sentinel's prev may change only if after was the tail sentinel,
       which we did not choose; therefore it stays unchanged. */
    assert(list.tail.prev == old_list.tail.prev);

    /* 9. Validity invariant must hold after the operation */
    assert(aws_linked_list_is_valid(&list));
}
