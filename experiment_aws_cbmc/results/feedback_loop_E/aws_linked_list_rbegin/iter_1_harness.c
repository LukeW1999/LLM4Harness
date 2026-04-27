#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rbegin_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Keep the old state of the list */
    struct aws_linked_list old_list = list;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));

    /* 1. Changed fields (from Doxygen) */
    /* The function returns the last element in the list, so no fields in the list itself change */

    /* 2. Unchanged fields (implied — Doxygen rarely lists these) */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 3. Both return paths */
    /* The function does not have a failure path as it simply returns a pointer */
    assert(result == list.tail.prev);

    /* 4. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    if (result != &list.head) {
        assert(aws_linked_list_node_prev_is_valid(result));
    }
}
