#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_front_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate at least one node to ensure the list is non‑empty */
    struct aws_linked_list_node *first = malloc(sizeof(*first));
    __CPROVER_assume(first != NULL);
    aws_linked_list_node_reset(first);
    aws_linked_list_push_back(&list, first);

    /* Optionally add a few more nodes (nondeterministic count) */
    size_t extra = nondet_size_t();
    __CPROVER_assume(extra < 5);
    for (size_t i = 0; i < extra; ++i) {
        struct aws_linked_list_node *n = malloc(sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_node_reset(n);
        aws_linked_list_push_back(&list, n);
    }

    /* Ground‑truth preconditions */
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Capture the list state for frame‑condition checks */
    struct aws_linked_list old_list = list;

    /* Call the function under test */
    struct aws_linked_list_node *front = aws_linked_list_front(&list);

    /* Post‑condition 1: return value correctness */
    assert(front == list.head.next);
    assert(front != &list.tail);
    assert(front->prev == &list.head);

    /* Post‑condition 2: list structural invariants unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* Post‑condition 3: list remains valid */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    return 0;
}
