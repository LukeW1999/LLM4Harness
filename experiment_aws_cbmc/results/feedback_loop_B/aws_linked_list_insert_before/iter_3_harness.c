#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    /* Assume the nodes are part of the list */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&before, sizeof(before)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&to_add, sizeof(to_add)));

    /* Assume the nodes are properly initialized */
    __CPROVER_assume(aws_linked_list_node_is_bounded(&before));
    __CPROVER_assume(aws_linked_list_node_is_bounded(&to_add));

    /* Assume the nodes are not the same */
    __CPROVER_assume(&before != &to_add);

    /* Assume the list is not empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Assume the node 'before' is not the head of the list */
    __CPROVER_assume(before.prev != &list.head);

    /* Assume the previous node of 'before' is valid */
    __CPROVER_assume(before.prev != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(before.prev, sizeof(struct aws_linked_list_node)));
    __CPROVER_assume(aws_linked_list_node_is_bounded(before.prev));

    /* Link the previous node to the 'before' node */
    before.prev->next = &before;

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_before = before;
    struct aws_linked_list_node old_to_add = to_add;

    /* 3. Call function under test */
    aws_linked_list_insert_before(&before, &to_add);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* In this case, the function does not return a status code, so we assume it always succeeds */
    {
        /* 1. Changed fields */
        assert(to_add.next == &before);
        assert(to_add.prev == old_before.prev);
        assert(old_before.prev->next == &to_add);
        assert(before.prev == &to_add);

        /* 2. Unchanged fields */
        assert(before.next == old_before.next);
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to assert as the function only modifies the nodes involved */

    /* 6. Assert validity invariants always holds */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_is_bounded(&before));
    assert(aws_linked_list_node_is_bounded(&to_add));
}
