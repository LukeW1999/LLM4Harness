#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_back_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate at least one node and push it to the list */
    struct aws_linked_list_node *first = malloc(sizeof(*first));
    __CPROVER_assume(first != NULL);
    aws_linked_list_node_reset(first);
    aws_linked_list_push_back(&list, first);

    /* Optionally add a few more nodes (bounded) */
    size_t extra = nondet_uint();
    __CPROVER_assume(extra < 5);
    for (size_t i = 0; i < extra; ++i) {
        struct aws_linked_list_node *n = malloc(sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_node_reset(n);
        aws_linked_list_push_back(&list, n);
    }

    /* Ground‑truth precondition */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Snapshot of the list state before the call */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* Post‑condition 1: return value correctness */
    __CPROVER_assert(result != NULL,
                     "aws_linked_list_back should not return NULL when list is non‑empty");
    __CPROVER_assert(result == old_tail_prev,
                     "aws_linked_list_back must return the node pointed to by tail.prev");
    __CPROVER_assert(result->next == &list.tail,
                     "The returned node's next pointer must point to the list tail sentinel");

    /* Post‑condition 2: list structural invariants unchanged */
    __CPROVER_assert(list.head.next == old_list.head.next,
                     "list.head.next must remain unchanged");
    __CPROVER_assert(list.head.prev == old_list.head.prev,
                     "list.head.prev must remain unchanged");
    __CPROVER_assert(list.tail.next == old_list.tail.next,
                     "list.tail.next must remain unchanged");
    __CPROVER_assert(list.tail.prev == old_list.tail.prev,
                     "list.tail.prev must remain unchanged");
    __CPROVER_assert(!aws_linked_list_empty(&list),
                     "list must remain non‑empty after aws_linked_list_back");

    return 0;
}
