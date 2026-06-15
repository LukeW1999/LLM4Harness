#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    /* Initialize a linked list with head and tail sentinels */
    struct aws_linked_list list;
    list.head.prev = NULL;
    list.head.next = &list.tail;
    list.tail.prev = &list.head;
    list.tail.next = NULL;

    /* Ensure the list is initially valid as an empty list */
    assert(aws_linked_list_is_valid(&list));

    /* Create a node to insert */
    struct aws_linked_list_node to_add;

    /* We will insert after the head sentinel */
    struct aws_linked_list_node *after = &list.head;
    struct aws_linked_list_node *old_next = after->next;  // This is the tail sentinel

    /* Call the function under test */
    aws_linked_list_insert_after(after, &to_add);

    /* Postcondition 1: The list structure remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 2: The inserted node is correctly linked */
    assert(after->next == &to_add);
    assert(to_add.prev == after);
    assert(to_add.next == old_next);
    assert(old_next->prev == &to_add);

    /* Postcondition 3: Sentinel invariants hold */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* Postcondition 4: The list is still valid (deep validity not required for this simple case) */
    assert(aws_linked_list_is_valid_deep(&list));  /* Only needed if the list type requires deep validity */
}
