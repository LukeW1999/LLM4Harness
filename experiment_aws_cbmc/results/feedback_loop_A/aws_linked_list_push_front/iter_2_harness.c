#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    ensure_linked_list_node_is_allocated(&node);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;

    /* 3. Call function under test */
    aws_linked_list_push_front(&list, &node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* For aws_linked_list_push_front, there is no return value, so we assume it always succeeds */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_list.head.next);
    if (old_list.head.next != NULL) {
        assert(old_list.head.next->prev == &node);
    } else {
        assert(list.tail.prev == &node);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old_list.alloc);

    /* 6. Assert validity invariants always holds */
    assert(aws_linked_list_is_valid(&list));
}
