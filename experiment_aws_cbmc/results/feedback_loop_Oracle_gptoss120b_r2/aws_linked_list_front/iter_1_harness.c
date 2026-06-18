#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* nondet helpers */
size_t nondet_size_t(void);
struct aws_linked_list_node *nondet_aws_linked_list_node_ptr(void);

void aws_linked_list_front_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate at least one node and push it to the front */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    __CPROVER_assume(node1 != NULL);
    aws_linked_list_node_reset(node1);
    aws_linked_list_push_front(&list, node1);

    /* Optionally add a few more nodes (bounded to keep verification tractable) */
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

    /* Snapshot the list structure for frame‑condition checking */
    struct aws_linked_list old_list = list;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* Post‑condition 1: return value correctness */
    __CPROVER_assert(result == list.head.next,
                     "aws_linked_list_front should return list.head.next");
    __CPROVER_assert(result != NULL,
                     "aws_linked_list_front must not return NULL when list is non‑empty");
    __CPROVER_assert(result->prev == &list.head,
                     "returned node's prev must point to list head");

    /* Post‑condition 2: list invariants remain unchanged */
    __CPROVER_assert(!aws_linked_list_empty(&list),
                     "list must remain non‑empty after aws_linked_list_front");
    __CPROVER_assert(aws_linked_list_is_valid(&list),
                     "list must remain valid after aws_linked_list_front");

    /* Post‑condition 3: frame conditions (no unintended modifications) */
    __CPROVER_assert(old_list.head.prev == list.head.prev,
                     "list.head.prev must be unchanged");
    __CPROVER_assert(old_list.tail.next == list.tail.next,
                     "list.tail.next must be unchanged");
    __CPROVER_assert(old_list.tail.prev == list.tail.prev,
                     "list.tail.prev must be unchanged");
    __CPROVER_assert(old_list.head.next == list.head.next,
                     "list.head.next must be unchanged (same as result)");

    return 0;
}
