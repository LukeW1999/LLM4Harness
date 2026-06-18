#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_end_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    const struct aws_linked_list_node *rval = aws_linked_list_end(&list);

    /* 4. Assert postconditions */
    /* The function returns a pointer to the tail sentinel */
    assert(rval == &list->tail);

    /* The list itself is not modified (const parameter) */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 5. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
