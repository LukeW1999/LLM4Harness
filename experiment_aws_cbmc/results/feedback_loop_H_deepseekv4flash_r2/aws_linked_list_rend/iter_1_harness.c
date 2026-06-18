#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_rend_harness() {
    /* Non-deterministic linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Call the function */
    const struct aws_linked_list_node *rval = aws_linked_list_rend(&list);

    /* Assert that the return value is &list.head */
    assert(rval == &list.head);

    /* Assert that the list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
