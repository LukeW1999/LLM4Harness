#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Add a real node to the list so we have a non‑sentinel element */
    struct aws_linked_list_node real_node;
    aws_linked_list_node_reset(&real_node);
    aws_linked_list_push_back(&list, &real_node);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Choose a node that is part of the list (head or the real node) */
    struct aws_linked_list_node *node;
    if (nondet_bool()) {
        node = &list.head;
    } else {
        node = &real_node;
    }
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    /* 4. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* 5. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 6. Post‑conditions */

    /* 6.1. The function returns the next pointer of the input node */
    assert(result == old_node.next);

    /* 6.2. No fields of the list are modified */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6.3. No fields of the node are modified */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 6.4. Validity invariant holds after the call */
    assert(aws_linked_list_is_valid(&list));
}
