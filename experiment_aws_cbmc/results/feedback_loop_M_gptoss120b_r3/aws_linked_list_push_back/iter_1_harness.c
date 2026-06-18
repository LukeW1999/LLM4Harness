#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_push_back */
void aws_linked_list_push_back_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;   /* previous last element */
    struct aws_linked_list_node *old_head_next = list.head.next; /* previous first element */

    /* 3. Allocate a node to insert (non‑NULL) */
    struct aws_linked_list_node node;
    struct aws_linked_list_node *node_ptr = &node;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node_ptr);

    /* 5. Post‑condition assertions */

    /* Validity invariant must hold */
    assert(aws_linked_list_is_valid(&list));

    /* Tail sentinel now points to the new node */
    assert(list.tail.prev == node_ptr);

    /* New node links correctly */
    assert(node_ptr->next == &list.tail);
    assert(node_ptr->prev == old_last);

    /* The previous last node now points forward to the new node */
    assert(old_last->next == node_ptr);

    /* Sentinel invariants remain unchanged */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* Head next pointer changes only if the list was empty */
    if (old_last == &list.head) {
        /* List was empty: head.next should now be the new node */
        assert(list.head.next == node_ptr);
    } else {
        /* List was non‑empty: head.next remains the same as before */
        assert(list.head.next == old_head_next);
    }
}
