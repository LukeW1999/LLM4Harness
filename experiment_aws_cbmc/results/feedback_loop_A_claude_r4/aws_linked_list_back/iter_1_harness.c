#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_back_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* aws_linked_list_back requires the list to be non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 4. Assert postconditions */
    /* The function returns the element at the back of the list, which is tail.prev */
    assert(result == old_tail_prev);
    assert(result == list.tail.prev);

    /* 5. Assert unchanged fields */
    /* The list structure itself should not be modified */
    assert(list.head.next == old_head_next);
    assert(list.tail.prev == old_tail_prev);

    /* The result should not be NULL (list is non-empty) */
    assert(result != NULL);

    /* The result should not be the tail sentinel */
    assert(result != &list.tail);

    /* 6. Assert validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
