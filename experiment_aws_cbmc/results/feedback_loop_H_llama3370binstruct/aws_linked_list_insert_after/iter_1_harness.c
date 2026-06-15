#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node after;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node to_add;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node old_after = after;
    struct aws_linked_list_node old_to_add = to_add;

    /* 3. Call function under test */
    aws_linked_list_insert_after(&after, &to_add);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(to_add.prev == &after);
    assert(to_add.next == old_after.next);
    assert(old_after.next->prev == &to_add);
    assert(after.next == &to_add);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(after.prev == old_after.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
