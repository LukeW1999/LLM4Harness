#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_rbegin_harness() {
    /* 1. Declare and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    /* 4. Assert postconditions */
    /* Returned pointer is never NULL */
    assert(r != NULL);

    /* If the list is not empty, r must be the last real element (its next is tail) */
    if (!aws_linked_list_empty(&list)) {
        assert(r->next == &list.tail);
    }

    /* The list must not have been modified – every sentinel pointer unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
