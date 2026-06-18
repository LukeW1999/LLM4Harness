#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state — for immutability checks on the list itself */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 4. Assert postconditions */

    /* According to natural language: "Returns the element in the back of the list."
     * Implementation returns list->tail.prev.
     * In a valid non-empty list, tail.prev is non-NULL (points to the last node).
     * In an empty list, tail.prev equals the address of list.head. But since
     * aws_linked_list_back does not distinguish empty/non-empty, we just assert
     * that the result is the element before the tail sentinel.
     */
    assert(result == list.tail.prev);

    /* 5. Assert that list structure did NOT change (it's const, read-only) */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Assert validity invariants still hold after call */
    assert(aws_linked_list_is_valid(&list));
}
