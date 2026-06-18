#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_begin_harness() {
    /* 1. Declare and set up a valid linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state to check immutability */
    struct aws_linked_list old = list;

    /* 3. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* 4. Postcondition: return value must be the first node */
    assert(result == list.head.next);
    /* head.next is never NULL in a valid list (points to tail or a real node) */
    assert(result != NULL);

    /* 5. The list must be completely unchanged (read-only operation) */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. The list invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
