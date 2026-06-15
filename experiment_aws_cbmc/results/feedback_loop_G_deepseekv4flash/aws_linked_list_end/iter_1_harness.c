#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness() {
    /* Initialize list with non-deterministic allocated nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state for immutability checks */
    struct aws_linked_list old = list;

    /* Call the function under test */
    const struct aws_linked_list_node *end = aws_linked_list_end(&list);

    /* Postconditions:
     * 1. Returned pointer equals address of the list's tail member.
     * 2. List structure is unchanged (immutability).
     * 3. List remains valid.
     */

    assert(end == &list.tail);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
