#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool is_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_head = list.head.next;

    struct aws_linked_list_node *popped_node = aws_linked_list_pop_front(&list, aws_default_allocator());

    if (is_empty) {
        assert(popped_node == NULL);
        assert(aws_linked_list_is_valid(&list));
        assert(aws_linked_list_empty(&list));
    } else {
        assert(popped_node == old_head);
        assert(popped_node!= NULL);
        assert(aws_linked_list_is_valid(&list));
        assert(!aws_linked_list_empty(&list) || popped_node == &list.tail);
    }
}
