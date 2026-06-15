#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof *to_add);
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 3. Choose insertion point: the list head sentinel */
    struct aws_linked_list_node *after = &list.head;

    /* 4. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_next = after->next;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Post‑condition asserts */

    /* Changed fields */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == old_next);
    assert(old_next->prev == to_add);
    assert(aws_linked_list_node_is_in_list(to_add));

    /* Unchanged fields */
    assert(after->prev == old_after_prev);
    assert(list.head.prev == &list.tail);
    assert(list.tail.next == &list.head);

    /* Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
