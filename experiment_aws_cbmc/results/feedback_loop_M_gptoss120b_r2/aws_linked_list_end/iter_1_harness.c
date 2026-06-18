#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_end_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state for immutability checks */
    struct aws_linked_list old = list;

    /* 3. Call the function under test */
    const struct aws_linked_list_node *r = aws_linked_list_end(&list);

    /* 4. Post‑condition: returned pointer is the tail sentinel */
    assert(r == &list.tail);

    /* 5. Unchanged fields – the list must not be modified */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
