#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a valid 'after' node (use the head sentinel for simplicity) */
    struct aws_linked_list_node *after = &list.head;
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(after, sizeof(*after)));

    /* 3. Allocate a new node to add */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(to_add, sizeof(*to_add)));

    /* Ensure the node is not linked before insertion */
    to_add->next = NULL;
    to_add->prev = NULL;

    /* 4. Save old state for post‑condition checks */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node old_head_prev = list.head.prev;
    struct aws_linked_list_node old_tail_next = list.tail.next;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Assert changed fields */
    assert(after->next == to_add);                     /* after now points to the new node */
    assert(to_add->prev == after);                     /* new node's prev points back to after */
    assert(to_add->next == old_after_next);            /* new node's next is the former after->next */
    assert(old_after_next->prev == to_add);            /* former next node now points back to new node */

    /* 7. Assert unchanged fields */
    assert(after->prev == old_after_prev);             /* after's prev unchanged */
    assert(list.head.prev == old_head_prev);           /* list head's prev unchanged */
    assert(list.tail.next == old_tail_next);           /* list tail's next unchanged */

    /* 8. Assert overall validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
