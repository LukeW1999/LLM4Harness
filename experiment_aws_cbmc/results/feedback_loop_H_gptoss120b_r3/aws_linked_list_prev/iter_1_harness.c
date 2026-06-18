#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare a node (stand‑alone, not necessarily part of the list) */
    struct aws_linked_list_node node_obj;
    /* nondet initialize the prev pointer */
    node_obj.prev = (struct aws_linked_list_node *)nondet_uint64_t();
    node_obj.next = (struct aws_linked_list_node *)nondet_uint64_t();

    struct aws_linked_list_node *node = &node_obj;

    /* 3. Save old state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list old_list = list;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Assert postconditions */
    /* The function returns the previous pointer unchanged */
    assert(result == old_prev);
    /* No fields of the node are modified */
    assert(node->prev == old_prev);
    assert(node->next == node_obj.next); /* next is unchanged */

    /* The list is unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
}
