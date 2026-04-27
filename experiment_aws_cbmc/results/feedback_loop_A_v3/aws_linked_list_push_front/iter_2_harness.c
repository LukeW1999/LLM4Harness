#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_push_front_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_linked_list_is_bounded(&list, MAX_LINKED_LIST_ITEM_ALLOCATION));
    __CPROVER_assume(aws_linked_list_node_is_bounded(&node));

    /* 3. Allocate pointer members */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_node_is_allocated(&node);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Save old state (for checking immutability) */
    struct aws_linked_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)&list, sizeof(struct aws_linked_list), &old_byte);

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    /* No specific preconditions for aws_linked_list_push_front */

    /* 7. Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* 8. Assert postconditions (from Ensures: annotations) */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old.head.next);
    if (old.head.next != NULL) {
        assert(old.head.next->prev == &node);
    }
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));  // invariant always holds
}
