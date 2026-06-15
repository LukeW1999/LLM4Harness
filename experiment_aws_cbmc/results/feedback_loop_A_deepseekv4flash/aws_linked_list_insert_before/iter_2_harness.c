#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness() {
    /* Create a valid linked list with at least one node (before) */
    struct aws_linked_list list;
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    /* Initialize the list with a sentinel head and one node 'before' */
    list.head.next = &before;
    list.head.prev = &before;
    before.next = &list.head;
    before.prev = &list.head;

    /* Ensure the list is valid */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Save old state */
    struct aws_linked_list_node old_before = before;
    struct aws_linked_list_node old_to_add = to_add;

    /* Call the function */
    aws_linked_list_insert_before(&before, &to_add);

    /* Postconditions */
    /* 1. to_add is inserted before before */
    assert(to_add.next == &before);
    assert(to_add.prev == before.prev);
    assert(before.prev->next == &to_add);
    assert(before.prev == &to_add);

    /* 2. before.next remains unchanged */
    assert(before.next == old_before.next);

    /* 3. List invariants remain valid */
    assert(aws_linked_list_is_valid_deep(&list));

    /* 4. The list still contains the same nodes (except added to_add) */
    assert(list.head.next == &to_add);
    assert(list.head.prev == &before);
}
