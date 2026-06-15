#include <assert.h>
#include <stdlib.h>
#include <aws/common/linked_list.h>

#define MAX_NODES 5

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_NODES);
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&list, node);
    }

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* Postconditions */
    assert(result == &list.tail);
    assert(result == &old.tail);

    /* The list must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));
}
