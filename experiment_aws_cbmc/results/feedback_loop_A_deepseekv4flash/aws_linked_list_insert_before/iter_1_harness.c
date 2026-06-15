#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness() {
    /* Non-deterministic inputs */
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    /* We need to ensure that before is part of a valid linked list.
     * For simplicity, we create a minimal valid list with before as a node.
     * We'll set up a list with head, before, and tail.
     */
    struct aws_linked_list_node head;
    struct aws_linked_list_node tail;
    head.next = &before;
    head.prev = NULL; /* head's prev is not used in a valid list */
    before.next = &tail;
    before.prev = &head;
    tail.next = NULL; /* tail's next is not used in a valid list */
    tail.prev = &before;

    /* Ensure the list is valid */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&((struct aws_linked_list){.head = head, .tail = tail})));

    /* Save old state */
    struct aws_linked_list_node old_before = before;
    struct aws_linked_list_node old_to_add = to_add;

    /* Call the function */
    aws_linked_list_insert_before(&before, &to_add);

    /* Postconditions */
    /* 1. Changed fields */
    assert(to_add.next == &before);
    assert(to_add.prev == before.prev);
    assert(before.prev->next == &to_add);
    assert(before.prev == &to_add);

    /* 2. Unchanged fields (implied) */
    /* before.next should remain unchanged */
    assert(before.next == old_before.next);
    /* to_add's other fields (if any) are not modified by this function */
    /* The list structure beyond the immediate neighbors should be unchanged */
    /* head.next should still point to before (since we inserted before before) */
    assert(head.next == &before);
    /* tail.prev should still point to before (since we inserted before before) */
    assert(tail.prev == &before);

    /* 3. No return value, so no success/failure paths */

    /* 4. Validity invariants */
    assert(aws_linked_list_is_valid_deep(&((struct aws_linked_list){.head = head, .tail = tail})));
}
