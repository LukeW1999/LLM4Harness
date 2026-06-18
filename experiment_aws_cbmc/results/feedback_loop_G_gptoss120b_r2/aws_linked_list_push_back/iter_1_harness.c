#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness() {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = old.tail.prev; /* previous last element (may be head sentinel) */

    /* 3. Allocate a non‑NULL node to insert */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Post‑condition: list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* 6. Changed fields */
    assert(list.tail.prev == node);          /* new node is the last element */
    assert(node->next == &list.tail);        /* node points to tail sentinel */
    assert(node->prev == old_last);          /* node links back to previous last element */

    /* 7. Unchanged sentinel fields */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* 8. Linkage of the previous last element (if any) */
    if (old_last != &list.head) {
        /* list was non‑empty: previous last node must now point forward to the new node */
        assert(old_last->next == node);
    } else {
        /* list was empty: head.next should now point to the new node */
        assert(list.head.next == node);
    }
}
