#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&before));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&before));
    __CPROVER_assume(to_add.next == NULL);
    __CPROVER_assume(to_add.prev == NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node old_before = before;

    /* 3. Call function under test */
    __CPROVER_assume(old_before.prev != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&before));
    aws_linked_list_insert_before(&before, &to_add);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(to_add.next == &before);
    assert(to_add.prev == old_before.prev);
    assert(before.prev == &to_add);
    if (old_before.prev != NULL) {
        assert(old_before.prev->next == &to_add);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(before.next == old_before.next);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_node_next_is_valid(&to_add));
    if (to_add.prev != NULL) {
        assert(aws_linked_list_node_prev_is_valid(&to_add));
    }
    if (before.prev != NULL) {
        assert(aws_linked_list_node_prev_is_valid(&before));
    }
    if (old_before.prev != NULL) {
        assert(aws_linked_list_node_next_is_valid(old_before.prev));
    }
    assert(aws_linked_list_node_is_valid(&to_add));
    assert(aws_linked_list_node_is_valid(&before));
}
