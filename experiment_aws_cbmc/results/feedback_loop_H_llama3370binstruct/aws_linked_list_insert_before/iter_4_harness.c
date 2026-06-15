#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
    bool success = aws_linked_list_insert_before(&list, &before, &to_add);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (success) {
        assert(to_add.next == &before);
        assert(to_add.prev == before.prev);
        if (before.prev != NULL) {
            assert(before.prev->next == &to_add);
        }
        if (before.prev == NULL) {
            assert(list.head == &to_add);
        }
    } else {
        assert(list.head == old_list.head);
        assert(list.tail == old_list.tail);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    if (before.next == NULL) {
        assert(list.tail == old_list.tail);
    }

    /* 7. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));

    /* 8. Test edge cases */
    before.prev = &to_add;
    before.next = &to_add;
    to_add.prev = &before;
    to_add.next = &before;
    success = aws_linked_list_insert_before(&list, &before, &to_add);
    assert(!success);

    /* 9. Test inserting at head */
    before.prev = NULL;
    before.next = NULL;
    to_add.prev = NULL;
    to_add.next = NULL;
    list.head = &before;
    list.tail = &before;
    before.prev = NULL;
    before.next = NULL;
    success = aws_linked_list_insert_before(&list, &before, &to_add);
    assert(success);
    assert(list.head == &to_add);
    assert(list.tail == &before);

    /* 10. Test inserting at tail */
    before.prev = NULL;
    before.next = NULL;
    to_add.prev = NULL;
    to_add.next = NULL;
    list.head = &before;
    list.tail = &before;
    before.prev = NULL;
    before.next = NULL;
    success = aws_linked_list_insert_before(&list, &before, &to_add);
    assert(success);
    assert(list.head == &to_add);
    assert(list.tail == &before);
}
