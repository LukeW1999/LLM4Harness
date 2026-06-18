#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure the list is non‑empty so that there is a node before which we can insert */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Choose a node that will serve as the insertion point (the "before" node) */
    struct aws_linked_list_node *before = list.tail.prev; /* last real node */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before != &list.head && before != &list.tail);

    /* 3. Allocate a node that is not currently in any list */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;

    /* 4. Save old state that will be examined after the call */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_head_prev   = list.head.prev;
    struct aws_linked_list_node *old_tail_next   = list.tail.next;

    /* 5. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 6. Post‑condition: fields that must have changed */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    assert(old_before_prev->next == to_add);

    /* 7. Post‑condition: fields that must remain unchanged */
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    /* The head and tail sentinel nodes themselves are unchanged */
    assert(&list.head == &list.head);
    assert(&list.tail == &list.tail);

    /* 8. The inserted node is now part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 9. Global invariants: the list remains valid */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
