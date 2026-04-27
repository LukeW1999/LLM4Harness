#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    // Initialize the nodes with non-deterministic values
    before.next = (struct aws_linked_list_node *)nondet_ptr();
    before.prev = (struct aws_linked_list_node *)nondet_ptr();
    to_add.next = (struct aws_linked_list_node *)nondet_ptr();
    to_add.prev = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the list structure is valid
    ensure_linked_list_is_allocated((struct aws_linked_list *)nondet_ptr(), nondet_size_t());

    // Additional assumptions to ensure the previous node of 'before' is valid
    assume(before.prev != NULL);
    assume(before.next != NULL);
    assume(to_add.prev != NULL);
    assume(to_add.next != NULL);

    // Ensure the next and prev pointers form a valid doubly linked list
    assume(before.prev->next == &before);
    assume(before.next->prev == &before);

    // Save old states
    struct aws_linked_list_node old_before = before;
    struct aws_linked_list_node old_to_add = to_add;

    // Call the function under test
    aws_linked_list_insert_before(&before, &to_add);

    // Assertions for success path
    assert(before.prev == &to_add);
    assert(to_add.next == &before);
    assert(to_add.prev == old_before.prev);
    assert(old_before.prev->next == &to_add);

    // Assertions for frame conditions
    assert(before.next == old_before.next);
    assert(to_add.next == &before || to_add.next == old_to_add.next);
    assert(to_add.prev == old_before.prev || to_add.prev == old_to_add.prev);

    // Assertions for validity invariants
    assert(before.next->prev == &before);
    assert(before.prev->next == &before);
    assert(to_add.next->prev == &to_add);
    assert(to_add.prev->next == &to_add);
}
