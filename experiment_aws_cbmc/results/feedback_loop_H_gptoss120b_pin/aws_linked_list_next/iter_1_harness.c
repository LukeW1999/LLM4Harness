#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node to query – use the list head sentinel (non‑NULL) */
    struct aws_linked_list_node *node = &list.head;

    /* 3. Save old state of the list */
    struct aws_linked_list old = list;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Post‑conditions: result must be the next pointer of the input node */
    assert(result == node->next);

    /* 6. The list must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 7. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
}
