#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node that is readable */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(node, sizeof *node));

    /* 3. Link the node into the list (as the sole element) */
    node->prev = &list.head;
    node->next = &list.tail;
    list.head.next = node;
    list.tail.prev = node;

    /* 4. Ensure the list is still valid after linking */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Save old state */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list old_list = list;

    /* 6. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 7. Postconditions */
    assert(result == old_node.next);
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 8. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
