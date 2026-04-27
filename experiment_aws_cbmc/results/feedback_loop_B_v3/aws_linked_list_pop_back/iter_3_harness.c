#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* 2. Bound the structure (limits CBMC state space) */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_node_is_allocated(&node);

    /* 3. Assume validity precondition (infer from how function uses the struct) */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 4. Save old state (for checking immutability) */
    struct aws_linked_list old = list;

    /* 5. Assume function-specific preconditions (infer from code logic) */
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(list.tail.prev != NULL);
    __CPROVER_assume(list.tail.prev->next == &list.tail);
    __CPROVER_assume(list.tail.prev->prev != NULL);

    /* 6. Call function under test */
    aws_linked_list_pop_back(&list, &node);

    /* 7. Assert postconditions (infer from what the function guarantees) */
    assert(aws_linked_list_is_valid(&list));
    assert(node.next == NULL);
    assert(node.prev == NULL);
    if (old.tail.prev == &old.head) {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        assert(list.tail.prev == old.tail.prev->prev);
        assert(old.tail.prev->prev->next == &list.tail);
    }
}
