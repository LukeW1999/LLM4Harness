#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_linked_list_rbegin_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_linked_list list;

    aws_linked_list_init(&list);

    /* nondeterministically add up to 4 nodes */
    size_t max_nodes = nondet_uint() % 5;
    for (size_t i = 0; i < max_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&list, node);
    }

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list list_old = list;

    struct aws_linked_list_node *r = aws_linked_list_rbegin(&list);

    /* Frame condition: the list structure must remain unchanged */
    assert(list.head.next == list_old.head.next);
    assert(list.head.prev == list_old.head.prev);
    assert(list.tail.next == list_old.tail.next);
    assert(list.tail.prev == list_old.tail.prev);

    /* rbegin should return the node pointed to by tail.prev */
    assert(r == list.tail.prev);

    if (aws_linked_list_empty(&list)) {
        /* Empty list: tail.prev points to the dummy head */
        assert(r == &list.head);
    } else {
        /* Non‑empty list: rbegin points to a real node */
        assert(r != &list.head);
        assert(r != &list.tail);
        assert(aws_linked_list_node_is_in_list(r));
    }
}
