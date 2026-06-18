#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node to add (not currently in any list) */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;

    /* 3. Choose a node that is already in the list to insert before.
       Use the list's tail sentinel, which is always present. */
    struct aws_linked_list_node *before = &list.tail;

    /* 4. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_prev_next = old_before_prev->next;

    /* 5. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 6. Post‑condition: fields that must have changed */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    assert(old_before_prev->next == to_add);

    /* 7. Post‑condition: fields that must remain unchanged */
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    if (before != old.head.next) {
        assert(list.head.next == old.head.next);
    }
    if (before != &list.tail) {               /* tail.prev changes only when inserting before tail */
        assert(list.tail.prev == old.tail.prev);
    }

    /* 8. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
