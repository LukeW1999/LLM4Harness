#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node pointer that is readable */
    struct aws_linked_list_node *node = nondet_uint8_t() ? NULL : NULL; /* placeholder to silence unused warnings */
    node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(node, sizeof *node));

    /* 3. Optionally link the node into the list (or leave it unrelated) */
    /* For safety, we allow any configuration but keep the list valid */
    /* No modifications to the list are performed here. */

    /* 4. Save old state */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list old_list = list;

    /* 5. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 6. Postconditions */

    /* The function must return the next pointer stored in the node */
    assert(result == old_node.next);

    /* The node itself must remain unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* The list must remain unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 7. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
