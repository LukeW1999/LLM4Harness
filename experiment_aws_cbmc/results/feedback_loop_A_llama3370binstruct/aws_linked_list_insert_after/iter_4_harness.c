#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;

    /* 3. Assume after is in the list */
    __CPROVER_assume(aws_linked_list_node_is_valid(&after));
    __CPROVER_assume(aws_linked_list_node_is_in_list(&list, &after));

    /* 4. Initialize to_add node */
    to_add.prev = NULL;
    to_add.next = NULL;

    /* 5. Call function under test */
    bool success = aws_linked_list_insert_after(&list, &after, &to_add);

    /* 6. Assert postconditions for BOTH success and failure paths */
    if (success) {
        assert(to_add.prev == &after);
        if (after.next != NULL) {
            assert(to_add.next == after.next);
            assert(after.next->prev == &to_add);
        } else {
            assert(list.tail == &to_add);
        }
        assert(aws_linked_list_is_valid(&list));
    } else {
        assert(aws_linked_list_is_valid(&list));
        assert(list.head == old_list.head);
        assert(list.tail == old_list.tail);
    }

    /* 7. Assert fields that must NOT change regardless of result */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
}

int main() {
    aws_linked_list_insert_after_harness();
    return 0;
}
