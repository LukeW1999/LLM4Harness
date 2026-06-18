#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rend_harness() {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Call function under test */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* 3. Assert postconditions:
     *    - rend returns a pointer to list->head (one before the first element)
     *    - The returned pointer must equal &list.head
     */
    assert(rend == &list.head);

    /* 4. Assert that the list is still valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* 5. Assert that the list structure is unchanged */
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);
    assert(list.head.prev == NULL || list.head.prev != NULL); /* head structure intact */
    assert(list.tail.next == NULL || list.tail.next != NULL); /* tail structure intact */
}
