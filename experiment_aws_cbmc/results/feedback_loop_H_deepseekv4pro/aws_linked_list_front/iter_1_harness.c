#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;

    /* Allocate and bound the list, ensuring it has a valid structure */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy of the list to check immutability */
    struct aws_linked_list old_list = list;

    /* Call the function under test */
    struct aws_linked_list_node *front = aws_linked_list_front(&list);

    /* 1. The list must remain valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* 2. The list structure (head and tail sentinels) must be unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 3. The returned pointer must be the same as the old head's next pointer */
    assert(front == old_list.head.next);

    /* 4. The returned node must be non-NULL (guaranteed by validity and list structure) */
    assert(front != NULL);
}
