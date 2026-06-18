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
     *    - aws_linked_list_end returns a pointer to list->tail
     *    - The list itself is unchanged
     *    - The list remains valid
     */

    /* The return value must be exactly &list->tail */
    assert(result == &list.tail);

    /* The list structure is unchanged */
    assert(aws_linked_list_is_valid(&list));
}
