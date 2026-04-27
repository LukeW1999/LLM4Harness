// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_pop_front returns a non-null value (indicating success):
//   - list.head.next: CHANGES to the next node after the original front node
//   - The node returned: CHANGES to have next and prev pointers set to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_pop_front returns NULL (indicating failure):
//   - list.head.next: UNCHANGED
//   - The node returned: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.prev: CHANGED on success, UNCHANGED on failure
//     - tail.next: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Save old state
    struct aws_linked_list old_list = list;

    struct aws_linked_list_node *front_node = aws_linked_list_pop_front(&list);

    // Assertions for success path
    if (front_node != NULL) {
        assert(list.head.next == old_list.head.next->next);
        assert(list.tail.prev == (aws_linked_list_empty(&list) ? &list.head : old_list.tail.prev));
        assert(front_node->next == NULL);
        assert(front_node->prev == NULL);
    } else {
        // Assertions for failure path
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
    }

    // Frame conditions
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    // Validity invariants
    assert(aws_linked_list_is_valid(&list));
}
