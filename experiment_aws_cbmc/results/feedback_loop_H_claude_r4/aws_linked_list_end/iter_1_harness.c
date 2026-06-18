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
     *    - Returns a pointer to one past the last element (i.e., &list->tail)
     *    - The returned pointer must equal &list.tail
     */
    assert(result == &list.tail);

    /* 4. Assert that the list structure is unchanged */
    assert(aws_linked_list_is_valid(&list));

    /* 5. Assert that the returned pointer is non-null and readable */
    assert(result != NULL);
}
