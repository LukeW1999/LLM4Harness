#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Declare a node (could be any node, not necessarily in the list) */
    struct aws_linked_list_node node;
    /* nondeterministically set its fields */
    node.next = (struct aws_linked_list_node *)nondet_uint64_t();
    node.prev = (struct aws_linked_list_node *)nondet_uint64_t();

    /* 3. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* 5. Postconditions: result must be the previous pointer of the input node */
    assert(result == old_node.prev);

    /* 6. Unchanged fields of the list */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 7. Unchanged fields of the node */
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);

    /* 8. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
