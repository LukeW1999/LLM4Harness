#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_push_back_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* 2. Bound the structure (limits CBMC state space) */
    /* No specific bounding function for aws_linked_list, so we assume reasonable sizes */
    size_t max_length = nondet_size_t();
    __CPROVER_assume(max_length > 0 && max_length < MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 3. Allocate pointer members */
    ensure_linked_list_is_allocated(&list, max_length);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Save old state (for checking immutability) */
    struct aws_linked_list old_list = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)&list, sizeof(struct aws_linked_list), &old_byte);

    /* 6. Assume function-specific preconditions */
    __CPROVER_assume(&node != NULL);

    /* 7. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 8. Assert postconditions (both branches) */
    assert(aws_linked_list_is_valid(&list));  // invariant always holds
    assert(list.tail.prev == &node);          // node is the new last element
    assert(node.prev == old_list.tail.prev);  // old last is now second-to-last
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));

    /* Check immutability of unchanged parts */
    assert_byte_from_buffer_matches((uint8_t *)&list, &old_byte);
}
