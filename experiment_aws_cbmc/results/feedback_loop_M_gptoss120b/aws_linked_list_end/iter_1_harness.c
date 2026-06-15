#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* CBMC harness for aws_linked_list_end */
void aws_linked_list_end_harness(void) {
    /* 1. Declare and allocate a linked list with bounded length */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state for immutability checks */
    struct aws_linked_list old = list;

    /* 3. Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* 4. Postconditions */

    /* The function must return a pointer to the list's tail node */
    assert(result == &list.tail);

    /* The list must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 5. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
