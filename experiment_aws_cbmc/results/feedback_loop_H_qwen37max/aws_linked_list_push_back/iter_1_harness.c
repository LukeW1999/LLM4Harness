#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_last = old_list.tail.prev;

    aws_linked_list_push_back(&list, node);

    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_last);
    assert(old_last->next == node);

    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
}
