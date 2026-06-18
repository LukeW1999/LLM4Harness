#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old_list = list;

    struct aws_linked_list_node *ret = aws_linked_list_front(&list);

    /* The returned pointer must equal the original head->next */
    assert(ret == old_list.head.next);

    /* The list structure must not be mutated */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* The list invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
}
