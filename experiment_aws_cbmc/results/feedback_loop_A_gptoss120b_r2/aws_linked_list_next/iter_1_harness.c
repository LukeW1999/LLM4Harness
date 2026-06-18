#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that belongs to the list (could be head, tail, or first element) */
    struct aws_linked_list_node *node;
    if (nondet_bool()) {
        node = &list.head;
    } else if (nondet_bool()) {
        node = &list.tail;
    } else {
        node = aws_linked_list_begin(&list);
    }
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(node, sizeof *node));

    /* 3. Save old state */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list old_list = list;

    /* 4. Call function under test */
    struct aws_linked_list_node *r = aws_linked_list_next(node);

    /* 5. Assert post‑conditions */
    /* returned pointer equals the original next pointer */
    assert(r == old_node.next);
    /* the examined node itself is unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
    /* the surrounding list structure is otherwise unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
