#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* choose after as either head (including empty list) or first real node */
    struct aws_linked_list_node *after;
    if (nondet_bool()) {
        after = &list.head;
    } else {
        __CPROVER_assume(list.head.next != &list.tail); /* ensure non-empty */
        after = list.head.next;
    }

    /* allocate a new node for insertion */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    __CPROVER_assume(to_add != after); /* avoid self-insertion */

    /* save old state for postcondition checks */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next_next = old_after_next->next;

    aws_linked_list_insert_after(after, to_add);

    /* 1. Changed fields: to_add is now between after and old_after_next */
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(after->next == to_add);
    assert(old_after_next->prev == to_add);

    /* 2. Unchanged fields */
    assert(after->prev == old_after_prev);        /* after->prev unchanged */
    assert(old_after_next->next == old_after_next_next); /* node after old_after_next unchanged */

    /* 3. List validity invariant must hold */
    assert(aws_linked_list_is_valid(&list));
}
