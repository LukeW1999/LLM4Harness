#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_end_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* 4. Assert postconditions:
     *    aws_linked_list_end returns a pointer to list->tail (the sentinel end node).
     *    The function does not modify any fields of the list.
     */

    /* The return value must be exactly &list->tail */
    assert(result == &list.tail);

    /* The function never modifies any fields of the list */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
