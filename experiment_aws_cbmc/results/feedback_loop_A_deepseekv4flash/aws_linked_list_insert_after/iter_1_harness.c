#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    /* Establish a linked list with head and tail sentinels and one interior node */
    struct aws_linked_list list;
    struct aws_linked_list_node interior;
    struct aws_linked_list_node to_add;

    /* Initialize head sentinel (prev and next both point to itself) */
    list.head.prev = &list.head;
    list.head.next = &interior;

    /* Initialize tail sentinel (prev points to interior, next points to itself) */
    list.tail.prev = &interior;
    list.tail.next = &list.tail;

    /* Initialize the interior node to link head and tail */
    interior.prev = &list.head;
    interior.next = &list.tail;

    /* The node to insert is initially isolated */
    to_add.prev = NULL;
    to_add.next = NULL;

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Save the old next pointer of the interior node (it will be overwritten) */
    struct aws_linked_list_node *old_after_next = interior.next;

    /* Call the function under test */
    aws_linked_list_insert_after(&interior, &to_add);

    /* ---- Postconditions ---- */

    /* 1. Fields that change (per specification and implementation) */
    assert(interior.next == &to_add);
    assert(to_add.prev == &interior);
    assert(to_add.next == old_after_next); /* the old interior->next */
    assert(list.tail.prev == &to_add);     /* because old_after_next was &list.tail */

    /* 2. Fields that must NOT change (implicit invariants) */
    assert(list.head.next == &interior); /* head's next still points to the original first node */
    assert(list.head.prev == &list.head);
    assert(list.tail.next == &list.tail);
    assert(interior.prev == &list.head); /* interior's prev unchanged */

    /* 3. No failure path; always succeeds */
    /* (void-returning function; no success/failure distinction) */

    /* 4. Validity invariants */
    assert(aws_linked_list_is_valid_deep(&list));
}
