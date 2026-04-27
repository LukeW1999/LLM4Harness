#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    /* Assume before is a valid node in the list */
    __CPROVER_assume(before.next != NULL && before.prev != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&before));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&before));

    /* Assume to_add is a valid node not in the list */
    __CPROVER_assume(to_add.next == NULL && to_add.prev == NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_before = before;

    /* 3. Call function under test */
    aws_linked_list_insert_before(&before, &to_add);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since the function does not return a status, we assume it always succeeds */
    assert(to_add.next == &before);
    assert(to_add.prev == before.prev);
    assert(before.prev->next == &to_add);
    assert(before.prev == &to_add);

    /* 5. Assert fields that must NOT change regardless of result */
    /* The list head and tail should not change */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_is_in_list(&to_add));
    assert(aws_linked_list_node_is_in_list(&before));
}
