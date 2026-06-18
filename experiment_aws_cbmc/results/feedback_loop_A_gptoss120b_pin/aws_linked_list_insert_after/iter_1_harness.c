#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate two nodes */
    struct aws_linked_list_node *after = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(to_add != NULL);

    /* 3. Initialize nodes (reset) */
    after->next = NULL;
    after->prev = NULL;
    to_add->next = NULL;
    to_add->prev = NULL;

    /* 4. Insert 'after' into the list so that it is a valid list node */
    aws_linked_list_push_back(&list, after);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Save old state before the call */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next = after->next;          /* should be &list.tail */
    struct aws_linked_list_node *old_next_prev = old_after_next->prev;  /* should be after */

    /* 6. Call function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 7. Postconditions: changed fields */
    assert(to_add->prev == after);                /* to_add.prev set to after */
    assert(to_add->next == old_after_next);       /* to_add.next set to after's old next */
    assert(after->next == to_add);                /* after.next now points to to_add */
    assert(old_after_next->prev == to_add);       /* old next's prev now points to to_add */

    /* 8. Unchanged fields */
    assert(after->prev == old_after_prev);        /* after.prev unchanged */
    /* Nodes other than after, to_add, and old_after_next remain unchanged – no other nodes exist in this harness */

    /* 9. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
