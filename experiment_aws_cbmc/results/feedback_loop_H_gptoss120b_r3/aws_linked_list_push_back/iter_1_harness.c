#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* 2. Save old state BEFORE calling */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 4. Assert postconditions for success (function cannot fail) */
    /* Changed fields */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_last);
    assert(old_last->next == node);

    /* Unchanged sentinel fields */
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* 5. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
