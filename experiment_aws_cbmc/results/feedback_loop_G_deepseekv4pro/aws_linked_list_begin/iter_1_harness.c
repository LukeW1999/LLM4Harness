#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_begin_harness() {
    /* 1. Declare and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state for immutability check */
    struct aws_linked_list old_list = list;

    /* 3. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* 4. Postconditions */
    /* The returned pointer must not be NULL (head.next is always a valid sentinel or node) */
    assert(result != NULL);

    /* The function returns head.next exactly */
    assert(result == list.head.next);

    /* The list must remain completely unchanged (const parameter) */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 5. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
