#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rend_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    const struct aws_linked_list_node *rval = aws_linked_list_rend(&list);

    /* 4. Post‑conditions */
    /* The function must return a pointer to the list head sentinel */
    assert(rval == &list.head);

    /* 5. Unchanged fields – the list must be unmodified */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
