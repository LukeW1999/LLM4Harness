#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rend_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* 4. Assert postconditions:
     *    aws_linked_list_rend returns a pointer to one before the first element,
     *    which is &list->head (the sentinel head node).
     */
    assert(rend == &list.head);

    /* 5. Assert unchanged fields - the list itself must not be modified */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
