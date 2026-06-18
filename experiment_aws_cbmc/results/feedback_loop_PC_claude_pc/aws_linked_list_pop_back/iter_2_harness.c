#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *new_back = old_back->prev;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result == old_back);
    assert(list.tail.prev == new_back);
    assert(new_back->next == &list.tail);
}
