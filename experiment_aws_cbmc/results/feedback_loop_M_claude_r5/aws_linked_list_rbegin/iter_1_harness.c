#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness() {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    /* 4. Assert postconditions */
    /* rbegin returns list->tail.prev, which is the last element (or head if empty) */
    assert(result == old_tail_prev);
    assert(result == list.tail.prev);

    /* 5. Assert unchanged fields — the list structure must not be modified */
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
