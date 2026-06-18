#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Optionally add a single node to the list */
    struct aws_linked_list_node *extra_node = NULL;
    if (nondet_bool()) {
        extra_node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(extra_node != NULL);
        aws_linked_list_node_reset(extra_node);
        aws_linked_list_push_back(&list, extra_node);
        __CPROVER_assume(aws_linked_list_is_valid(&list));
        __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    }

    /* 3. Choose a node to query */
    const struct aws_linked_list_node *node;
    if (nondet_bool()) {
        node = &list.head;
    } else if (nondet_bool()) {
        node = &list.tail;
    } else {
        /* If we added an extra node, use it; otherwise fall back to head */
        node = extra_node ? extra_node : &list.head;
    }

    __CPROVER_assume(node != NULL);

    /* 4. Save old state */
    struct aws_linked_list old_list = list;
    const struct aws_linked_list_node *old_next = node->next;

    /* 5. Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 6. Post‑condition: result must be the original next pointer */
    assert(result == old_next);

    /* 7. Unchanged fields of the list (shallow copy) */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 8. The queried node itself must not be modified */
    assert(node->next == old_next);
    assert(node->prev == (node == &list.head ? old_list.head.prev :
                         node == &list.tail ? old_list.tail.prev :
                         (extra_node ? extra_node->prev : NULL)));

    /* 9. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
