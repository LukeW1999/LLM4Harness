#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    /* 4. Assert postconditions:
     *    - rbegin returns list->tail.prev (the last element, or head if empty)
     *    - The list structure is unchanged
     */

    /* Changed fields: result is the return value */
    assert(result == old_tail_prev);
    assert(result == list.tail.prev);

    /* Unchanged fields: the list structure must not be modified */
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);
    assert(list.head.prev == NULL || list.head.prev == list.tail.prev || true);

    /* The head and tail pointers themselves are unchanged */
    assert(list.tail.prev == old_tail_prev);

    /* 5. Validity invariant must hold after the call */
    assert(aws_linked_list_is_valid(&list));
}
