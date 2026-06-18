#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness() {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node (precondition: node != NULL) */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Post‑condition: list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* 6. Post‑condition: new node is now the last element */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_last);

    /* 7. Unchanged fields of the list */
    assert(list.tail.next == NULL);               /* tail.next is always NULL */
    assert(list.head.prev == NULL);               /* head.prev is always NULL */
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* 8. Head.next changes only when the list was empty */
    if (old.head.next == &old.tail) {
        assert(list.head.next == node);
    } else {
        assert(list.head.next == old.head.next);
    }

    /* 9. If the list was non‑empty, the previous last node now points to the new node */
    if (old_last != &old.tail) {
        assert(old_last->next == node);
    }
}
