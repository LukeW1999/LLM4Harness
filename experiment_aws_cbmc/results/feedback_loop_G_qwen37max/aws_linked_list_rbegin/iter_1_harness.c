#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);
    
    ensure_linked_list_is_allocated(list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(list));

    struct aws_linked_list old_list = *list;

    struct aws_linked_list_node *result = aws_linked_list_rbegin(list);

    /* 1. Changed fields / Return value */
    assert(result == list->tail.prev);
    
    /* 2. Unchanged fields (immutability) */
    assert(list->head.next == old_list.head.next);
    assert(list->head.prev == old_list.head.prev);
    assert(list->tail.next == old_list.tail.next);
    assert(list->tail.prev == old_list.tail.prev);

    /* 4. Validity invariants */
    assert(aws_linked_list_is_valid(list));
}
