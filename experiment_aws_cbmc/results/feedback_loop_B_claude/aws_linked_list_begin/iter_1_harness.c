#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* 4. Assert postconditions:
     *    aws_linked_list_begin returns list->head.next
     */
    assert(result == list.head.next);
    assert(result == old.head.next);

    /* 5. Assert unchanged fields — the function only reads, never writes */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));

    /* 7. The result must be non-NULL (head.next is always valid in a valid list) */
    assert(result != NULL);
}
