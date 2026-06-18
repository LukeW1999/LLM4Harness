#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_back_harness() {
    /* 1. Declare and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_back = list.tail.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *rval = aws_linked_list_back(&list);

    /* 4. Postconditions */
    /* The returned pointer must be the current back element */
    assert(rval == list.tail.prev);
    /* It must also be the same as it was before the call */
    assert(rval == old_back);

    /* 5. Unchanged fields */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
