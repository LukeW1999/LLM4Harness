#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    /* 2. Initialize before and to_add nodes */
    before.prev = NULL;
    before.next = NULL;
    to_add.prev = NULL;
    to_add.next = NULL;

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;

    /* 4. Call function under test */
    aws_linked_list_insert_before(&list, &before, &to_add);

    /* 5. Assert postconditions for BOTH success and failure paths */
    assert(to_add.next == &before);
    assert(to_add.prev == before.prev);
    if (before.prev != NULL) {
        assert(before.prev->next == &to_add);
    }
    if (before.prev == NULL) {
        assert(list.head == &to_add);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.tail == old_list.tail);

    /* 7. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
