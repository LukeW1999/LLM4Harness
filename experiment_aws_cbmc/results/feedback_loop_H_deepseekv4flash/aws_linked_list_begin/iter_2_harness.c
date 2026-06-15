#include <aws/common/linked_list.h>
#include <stdlib.h>
#include <assert.h>

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node head_sentinel, tail_sentinel;

    // Initialize sentinels for an empty list initially
    head_sentinel.next = &tail_sentinel;
    head_sentinel.prev = NULL;
    tail_sentinel.prev = &head_sentinel;
    tail_sentinel.next = NULL;
    list.head = &head_sentinel;
    list.tail = &tail_sentinel;

    // Add one node to make the list non-empty
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    node->next = &tail_sentinel;
    node->prev = &head_sentinel;
    head_sentinel.next = node;
    tail_sentinel.prev = node;

    // Call function under test
    struct aws_linked_list_node *rval = aws_linked_list_begin(&list);

    // Postconditions
    assert(rval == list.head.next);
    assert(list.head == &head_sentinel);
    assert(list.tail == &tail_sentinel);
    assert(list.head.next == node);
    assert(node->prev == &head_sentinel);
    assert(node->next == &tail_sentinel);
    assert(tail_sentinel.prev == node);
    assert(aws_linked_list_is_valid(&list));
}
