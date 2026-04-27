#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;

    /* 2. Assume preconditions */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&after, sizeof(after)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&to_add, sizeof(to_add)));

    /* Ensure 'after' is part of the list */
    struct aws_linked_list_node *old_after_next = after.next;
    struct aws_linked_list_node *old_after_prev = after.prev;
    __CPROVER_assume(old_after_next != NULL);
    __CPROVER_assume(old_after_prev != NULL);
    __CPROVER_assume(old_after_next->prev == &after);
    __CPROVER_assume(old_after_prev->next == &after);

    /* Ensure 'to_add' is not part of any list */
    __CPROVER_assume(to_add.next == NULL);
    __CPROVER_assume(to_add.prev == NULL);

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_after = after;
    struct aws_linked_list_node old_to_add = to_add;

    /* 3. Call function under test */
    aws_linked_list_insert_after(&after, &to_add);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* For this function, there is no failure path, so we only check success */
    {
        /* 1. Changed fields (from Doxygen) */
        assert(to_add.prev == &after);
        assert(to_add.next == old_after_next);
        assert(old_after_next->prev == &to_add);
        assert(after.next == &to_add);

        /* 2. Unchanged fields (implied — Doxygen rarely lists these) */
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
        assert(after.prev == old_after.prev);
        assert(old_after_next->next == old_list.tail.prev->next);
        assert(old_after_prev->prev == old_list.head.next->prev);

        /* 3. Both return paths */
        /* No return value to check, but we assume no failure */

        /* 4. Validity invariants */
        assert(aws_linked_list_is_valid(&list));
        assert(aws_linked_list_node_next_is_valid(&after));
        assert(aws_linked_list_node_prev_is_valid(&to_add));
        assert(aws_linked_list_node_next_is_valid(&to_add));
        assert(aws_linked_list_node_prev_is_valid(old_after_next));
    }
}
