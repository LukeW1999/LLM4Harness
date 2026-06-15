#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node *old_prev = node.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result!= NULL) {
        assert(result == node.prev);
    } else {
        assert(node.prev == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(node.next == old_node.next);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&node));
}
