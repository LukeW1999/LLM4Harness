#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev; /* previous last element */

    /* 3. Allocate a fresh node (stack‑allocated) */
    struct aws_linked_list_node node;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* 5. Post‑condition: list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* 6. Post‑condition: tail now points to the new node */
    assert(list.tail.prev == &node);

    /* 7. Post‑condition: new node links are correct */
    assert(node.next == &list.tail);
    assert(node.prev == old_last);

    /* 8. Post‑condition: previous last node (if any) now points forward to the new node */
    if (old_last != NULL) {
        assert(old_last->next == &node);
    } else {
        /* list was empty before the push_back */
        assert(list.head.next == &node);
    }

    /* 9. Unchanged fields (sentinel invariants) */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* 10. The rest of the list structure (other nodes) must be unchanged.
       Since we saved the whole list before the call, we can compare the
       head and tail sentinel nodes themselves. */
    assert(list.head.next == (old_last == NULL ? &node : old.head.next));
}
