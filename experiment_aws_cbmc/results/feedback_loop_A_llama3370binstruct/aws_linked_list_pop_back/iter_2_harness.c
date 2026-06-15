#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool is_empty = aws_linked_list_empty(&list);
    if (is_empty) {
        assert(aws_linked_list_pop_back(&list) == NULL);
    } else {
        struct aws_linked_list_node *old_tail = list.tail.prev;
        struct aws_linked_list_node *popped_node = aws_linked_list_pop_back(&list);
        assert(popped_node == old_tail);
        assert(aws_linked_list_is_valid(&list));
    }

    assert(aws_linked_list_is_valid(&list));
}

int main() {
    aws_linked_list_pop_back_harness();
    return 0;
}
