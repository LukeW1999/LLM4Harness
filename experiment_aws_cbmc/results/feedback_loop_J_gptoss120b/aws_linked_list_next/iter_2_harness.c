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

    /* 2. Allocate a node and insert it so that the list is non‑empty */
    struct aws_linked_list_node *extra_node = malloc(sizeof(*extra_node));
    __CPROVER_assume(extra_node != NULL);
    aws_linked_list_push_back(&list, extra_node);

    /* 3. Save the state of the list after insertion (the function under test must not modify it) */
    struct aws_linked_list old_list = list;

    /* 4. Choose a node to query – must be a valid node (head or the inserted node) */
    const struct aws_linked_list_node *test_node;
    if (nondet_bool()) {
        test_node = &list.head;
    } else {
        test_node = extra_node;
    }

    /* 5. Save the expected result (the next pointer before the call) */
    const struct aws_linked_list_node *expected_next = test_node->next;

    /* 6. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(test_node);

    /* 7. Post‑condition: the returned pointer must be the original next */
    assert(result == expected_next);

    /* 8. No side‑effects on the list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 9. No side‑effects on the extra node (if it was part of the list) */
    assert(extra_node->next == old_list.tail.prev->next);
    assert(extra_node->prev == old_list.tail.prev->prev);

    /* 10. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
