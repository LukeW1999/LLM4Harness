#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_linked_list_pop_back_harness() {
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
    __CPROVER_assume(aws_linked_list_node_is_valid(&node));

    /* 5. Add a node to the list */
    aws_linked_list_push_back(&list, &node);

    /* 6. Save old state (for checking immutability) */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)&list, sizeof(struct aws_linked_list), &old_byte);

    /* 7. Assume function-specific preconditions (from Requires: annotations) */
    /* No specific preconditions for pop_back */

    /* 8. Call function under test */
    struct aws_linked_list_node *popped_node = aws_linked_list_pop_back(&list);

    /* 9. Assert postconditions (from Ensures: annotations) */
    assert(popped_node == &node);
    assert(aws_linked_list_is_empty(&list));
    assert(aws_linked_list_is_valid(&list));
}
