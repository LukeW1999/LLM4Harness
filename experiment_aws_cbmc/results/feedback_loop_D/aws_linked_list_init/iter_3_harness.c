#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_init_harness() {
    struct aws_linked_list list;

    // Initialize the list to some arbitrary state
    list.head.next = (struct aws_linked_list_node *)nondet_ptr();
    list.head.prev = (struct aws_linked_list_node *)nondet_ptr();
    list.tail.prev = (struct aws_linked_list_node *)nondet_ptr();
    list.tail.next = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the list is allocated
    __CPROVER_assume(AWS_MEM_IS_VALID(&list, sizeof(struct aws_linked_list)));

    int result = aws_linked_list_init(&list);

    if (result == AWS_OP_SUCCESS) {
        // Assert success path changes
        assert(list.head.next == &list.tail);
        assert(list.head.prev == NULL);
        assert(list.tail.prev == &list.head);
        assert(list.tail.next == NULL);
    } else {
        // Assert failure path changes (no change expected)
        assert(list.head.next == (struct aws_linked_list_node *)nondet_ptr());
        assert(list.head.prev == (struct aws_linked_list_node *)nondet_ptr());
        assert(list.tail.prev == (struct aws_linked_list_node *)nondet_ptr());
        assert(list.tail.next == (struct aws_linked_list_node *)nondet_ptr());
    }

    // Assert validity invariant
    assert(aws_linked_list_is_valid(&list));
}
