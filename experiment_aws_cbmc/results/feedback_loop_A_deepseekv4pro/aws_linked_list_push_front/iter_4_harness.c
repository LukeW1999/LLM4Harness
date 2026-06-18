#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_linked_list_push_front_harness() {
    /* Allocate a valid linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create a new node that is not yet in any list */
    struct aws_linked_list_node new_node;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&new_node));

    /* Save state before insertion */
    struct aws_linked_list_node old_head_next = *list.head.next;
    struct aws_linked_list_node old_tail_prev = *list.tail.prev;

    /* Function under test */
    aws_linked_list_push_front(&list, &new_node);

    /* Post-conditions:
     * 1. The list remains valid.
     */
    assert(aws_linked_list_is_valid(&list));

    /* 2. The new node is now the head of the list */
    assert(list.head.next == &new_node);
    assert(new_node.prev == &list.head);

    /* 3. If the list was empty, the new node is also the tail */
    if (old_head_next.next == &list.tail) {
        assert(new_node.next == &list.tail);
        assert(list.tail.prev == &new_node);
    } else {
        /* 4. If not empty, the old first node now follows the new node */
        assert(new_node.next == &old_head_next);
        assert(old_head_next.prev == &new_node);
    }

    /* 5. Sentinel nodes remain unchanged */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
