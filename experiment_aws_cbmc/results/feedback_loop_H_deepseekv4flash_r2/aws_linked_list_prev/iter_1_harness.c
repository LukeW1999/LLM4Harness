#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_prev_harness() {
    /* Non-deterministic node pointer to test general case */
    struct aws_linked_list_node node;
    struct aws_linked_list_node *node_ptr = &node;

    /* Optionally, place node in a valid list to test connectivity */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure the list has at least one node (so we can test a real node) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Pick an arbitrary node: the first real node */
    struct aws_linked_list_node *real_node = list.head.next;
    __CPROVER_assume(real_node != &list.tail); /* not the sentinel tail */

    /* Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *real_node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(real_node);

    /* 1. Assert return value matches node->prev */
    assert(result == real_node->prev);

    /* 2. Assert node's fields are unchanged */
    assert(real_node->next == old_node.next);
    assert(real_node->prev == old_node.prev);

    /* 3. Assert the list structure is unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 4. Assert list validity still holds */
    assert(aws_linked_list_is_valid(&list));
}
