#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_linked_list list;

    aws_linked_list_init(&list);
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node = aws_linked_list_pop_front(&list);

    assert(aws_linked_list_is_valid(&list));

    if (node == NULL) {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    }
}
