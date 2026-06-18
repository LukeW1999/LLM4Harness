#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_end_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Call function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* 3. Assert postconditions:
     *    aws_linked_list_end returns a pointer to one past the last element,
     *    which is &list->tail.
     */
    assert(result == &list.tail);

    /* 4. Assert the list is still valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* 5. Assert that the list structure is unchanged (head and tail pointers) */
    /* The function is read-only, so head and tail linkage must be preserved */
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);
}
