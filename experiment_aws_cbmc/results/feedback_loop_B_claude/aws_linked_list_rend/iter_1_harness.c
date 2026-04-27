#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_rend_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* 4. Assert postconditions */

    /* The function returns a pointer to list->head */
    assert(rend == &list.head);

    /* The function does not modify the list at all */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The returned pointer is non-null (it points to a field of list) */
    assert(rend != NULL);

    /* The list remains valid after the call */
    assert(aws_linked_list_is_valid(&list));
}
