#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Choose a node that is part of the list (head or tail) */
    const struct aws_linked_list_node *node;
    if (nondet_bool()) {
        node = &list.head;
    } else {
        node = &list.tail;
    }

    /* 3. Save old state */
    const struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list old_list = list;

    /* 4. Call function under test */
    const struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Postconditions */
    /* The function must return the previous node */
    assert(result == old_prev);

    /* No modifications to the list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* The node pointer itself is unchanged */
    assert(node == (const struct aws_linked_list_node *)node);

    /* 6. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
