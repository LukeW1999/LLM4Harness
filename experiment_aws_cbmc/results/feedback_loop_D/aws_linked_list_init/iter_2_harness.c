#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_init_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list = list;

    // Initialize the list to some arbitrary state
    list.head.next = (struct aws_linked_list_node *)nondet_ptr();
    list.head.prev = (struct aws_linked_list_node *)nondet_ptr();
    list.tail.prev = (struct aws_linked_list_node *)nondet_ptr();
    list.tail.next = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the list is allocated
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    int result = aws_linked_list_init(&list);

    if (result == AWS_OP_SUCCESS) {
        // Assert success path changes
        assert(list.head.next == &list.tail);
        assert(list.head.prev == NULL);
        assert(list.tail.prev == &list.head);
        assert(list.tail.next == NULL);
    } else {
        // Assert failure path changes (no change expected)
        assert(list.head.next == old_list.head.next);
        assert(list.head.prev == old_list.head.prev);
        assert(list.tail.prev == old_list.tail.prev);
        assert(list.tail.next == old_list.tail.next);
    }

    // Assert validity invariant
    assert(aws_linked_list_is_valid(&list));
}
