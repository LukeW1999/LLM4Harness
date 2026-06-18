#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rend_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list_node old_head = list.head;
    struct aws_linked_list_node old_tail = list.tail;

    /* 3. Call function under test */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* 4. Assert postconditions */

    /* RETURN: rend must point to &list.head */
    assert(rend == &list.head);

    /* FRAME: list.head must not have changed */
    assert(list.head.next == old_head.next);
    assert(list.head.prev == old_head.prev);

    /* FRAME: list.tail must not have changed */
    assert(list.tail.next == old_tail.next);
    assert(list.tail.prev == old_tail.prev);

    /* INVARIANTS: list must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The returned pointer is non-null */
    assert(rend != NULL);

    /* The returned pointer is readable */
    assert(AWS_MEM_IS_READABLE(rend, sizeof(struct aws_linked_list_node)));
}
