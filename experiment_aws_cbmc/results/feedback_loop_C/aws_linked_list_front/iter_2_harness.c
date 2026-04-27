#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list;

    // Initialize old_list to be the same as list
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    old_list = list;

    // Ensure the list is allocated and valid
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Call the function under test
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    // === FRAME CONDITIONS ===
    // list.head.next: CHANGED on success, UNCHANGED on failure
    if (result != NULL) {
        assert(list.head.next == result);
    } else {
        assert(list.head.next == old_list.head.next);
    }

    // list.head.prev: UNCHANGED always
    assert(list.head.prev == old_list.head.prev);

    // list.tail.next: UNCHANGED always
    assert(list.tail.next == old_list.tail.next);

    // list.tail.prev: UNCHANGED always
    assert(list.tail.prev == old_list.tail.prev);

    // === VALIDITY INVARIANTS ===
    // aws_linked_list_is_valid(&list): YES (must hold after call)
    assert(aws_linked_list_is_valid(&list));
}
