#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rend_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state for immutability checks */
    struct aws_linked_list old = list;

    /* 3. Call the function under test */
    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    /* 4. Post‑condition: return value points to the head sentinel */
    assert(r == &list.head);

    /* 5. Unchanged fields: the list must remain exactly the same */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
