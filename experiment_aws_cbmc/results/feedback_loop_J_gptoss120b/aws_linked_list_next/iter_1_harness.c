#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state of the list */
    struct aws_linked_list old_list = list;

    /* 2. Allocate a node that may be inserted into the list */
    struct aws_linked_list_node *extra_node = malloc(sizeof(*extra_node));
    __CPROVER_assume(extra_node != NULL);
    /* Insert the node so that it becomes part of the list */
    aws_linked_list_push_back(&list, extra_node);

    /* Save old state of the inserted node */
    struct aws_linked_list_node old_extra_node = *extra_node;

    /* 3. Choose a node to query */
    const struct aws_linked_list_node *test_node;
    if (nondet_bool()) {
        test_node = &list.head;
    } else if (nondet_bool()) {
        test_node = &list.tail;
    } else {
        test_node = extra_node;
    }

    /* Save the expected result (the next pointer before the call) */
    const struct aws_linked_list_node *expected_next = test_node->next;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(test_node);

    /* 5. Post‑condition: the returned pointer must be the original next */
    assert(result == expected_next);

    /* 6. No side‑effects on the list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 7. No side‑effects on the extra node (if it was part of the list) */
    assert(extra_node->next == old_extra_node.next);
    assert(extra_node->prev == old_extra_node.prev);

    /* 8. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
