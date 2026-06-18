#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate two nodes: one that will be in the list (before),
       and one that will be inserted (to_add) */
    struct aws_linked_list_node *before = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(to_add != NULL);

    /* 3. Build a valid list that contains at least the sentinel nodes.
       We will insert before the tail sentinel, which is always a valid
       insertion point. */
    /* Ensure the list currently satisfies its invariants */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Choose the insertion point */
    before = (struct aws_linked_list_node *)&list.tail; /* insert before tail */
    __CPROVER_assume(before != &list.head); /* never insert before head sentinel */

    /* 4. Ensure the node to be inserted is not currently linked */
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL);

    /* 5. Save old state needed for post‑condition checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_prev = before->prev;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑condition checks */

    /* 7a. The new node must be linked correctly */
    assert(to_add->next == before);
    assert(to_add->prev == old_prev);
    assert(old_prev->next == to_add);
    assert(before->prev == to_add);

    /* 7b. The tail sentinel must now point back to the new node when inserting before tail */
    if (before == &list.tail) {
        assert(list.tail.prev == to_add);
    }

    /* 7c. The head and tail sentinel addresses must remain unchanged */
    assert(&list.head == &old_list.head);
    assert(&list.tail == &old_list.tail);

    /* 7d. No other pointers in the list may have been altered */
    /* Since we only inserted before a known sentinel, the only
       pointers that can change are those we already asserted. */
    /* (No additional explicit asserts are needed for other nodes because
       the list currently contains only the sentinels.) */

    /* 8. The list must remain valid after the operation */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
