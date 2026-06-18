#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node and nondeterministically set its fields */
    struct aws_linked_list_node *node = malloc(sizeof *node);
    __CPROVER_assume(node != NULL);
    /* nondet pointers for next/prev (could be NULL or any address) */
    node->next = (struct aws_linked_list_node *)nondet_uint64_t();
    node->prev = (struct aws_linked_list_node *)nondet_uint64_t();

    /* 3. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Postconditions */

    /* 5.1. Return value equals the original prev pointer */
    assert(result == old_node.prev);

    /* 5.2. Node fields unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 5.3. List fields unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 5.4. Validity invariant holds */
    assert(aws_linked_list_is_valid(&list));
}
