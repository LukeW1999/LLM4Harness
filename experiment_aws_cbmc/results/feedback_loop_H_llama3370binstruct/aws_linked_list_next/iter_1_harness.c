#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&next_node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&next_node));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node old_node = node;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == &next_node) {
        assert(node.next == &next_node);
    } else {
        assert(node.next == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(node.prev == old_node.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_node_next_is_valid(&node));
}
