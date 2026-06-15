#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *old_head = list.head.next;

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(result == list.head.next);
    assert(list.head.next == old_head);
    assert(list.tail.prev == list.tail.prev); /* tail remains unchanged */
    assert(aws_linked_list_is_valid(&list));
}

int main() {
    aws_linked_list_begin_harness();
    return 0;
}
