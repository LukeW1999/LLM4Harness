#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_front_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *rval = aws_linked_list_front(&list);

    /* 4. Post‑condition: return value is the first node */
    assert(rval == list.head.next);
    assert(rval == old.head.next);

    /* 5. Unchanged fields */
    assert(list.head.prev == old.head.prev);
    assert(list.head.next == old.head.next);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);

    /* 6. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
