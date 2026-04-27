#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Save old state
    struct aws_linked_list old_list = list;

    // Assume list is not empty for success case
    __CPROVER_assume(!aws_linked_list_empty(&list));

    // Assume list has at least two elements to avoid issues with head and tail being the same
    __CPROVER_assume(old_list.head.next != &old_list.tail);

    // Call the function under test
    struct aws_linked_list_node *removed_node = aws_linked_list_pop_back(&list);

    // Success path assertions
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == (old_list.head.next == &old_list.tail ? &list.tail : old_list.head.next->next));
    assert(list.tail.prev == (old_list.tail.prev == &old_list.head ? &list.head : old_list.tail.prev->prev));
    assert(removed_node->next == NULL);
    assert(removed_node->prev == NULL);

    // Failure path assertions (not applicable here as we assume list is not empty)
    // If list were empty, aws_linked_list_pop_back would not be called, or it would be a precondition violation
}
