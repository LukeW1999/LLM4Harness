#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_remove_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION, aws_default_allocator());
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *node = list.head.next;
    if (node != NULL) {
        aws_linked_list_remove(node);
    }

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (node != NULL) {
        assert(node->next == NULL);
        assert(node->prev == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
