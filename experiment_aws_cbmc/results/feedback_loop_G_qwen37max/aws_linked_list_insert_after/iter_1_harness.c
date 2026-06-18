#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *after = &list.head;
    struct aws_linked_list_node to_add;

    struct aws_linked_list_node *old_after_next = after->next;

    aws_linked_list_insert_after(after, &to_add);

    assert(to_add.prev == after);
    assert(to_add.next == old_after_next);
    assert(old_after_next->prev == &to_add);
    assert(after->next == &to_add);
}
