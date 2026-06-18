#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_back_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* create a non‑empty list */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    aws_linked_list_push_back(&list, node);

    /* ground‑truth precondition */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* snapshot pre‑state for frame condition checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_back(&list);

    /* post‑condition assertions */
    __CPROVER_assert(rval != NULL,
                     "aws_linked_list_back: return value must be non‑NULL when list is non‑empty");
    __CPROVER_assert(rval == list.tail.prev,
                     "aws_linked_list_back: return value must equal list.tail.prev");
    __CPROVER_assert(rval->next == &list.tail,
                     "aws_linked_list_back: returned node's next must point to list.tail");
    __CPROVER_assert(aws_linked_list_is_valid(&list),
                     "aws_linked_list_back: list must remain valid after call");

    /* frame condition checks (no memory beyond contract modified) */
    __CPROVER_assert(old_list.head.next == list.head.next,
                     "aws_linked_list_back: list.head.next unchanged");
    __CPROVER_assert(old_list.head.prev == list.head.prev,
                     "aws_linked_list_back: list.head.prev unchanged");
    __CPROVER_assert(old_list.tail.next == list.tail.next,
                     "aws_linked_list_back: list.tail.next unchanged");
    __CPROVER_assert(old_list.tail.prev == list.tail.prev,
                     "aws_linked_list_back: list.tail.prev unchanged");
    __CPROVER_assert(old_node.next == node->next && old_node.prev == node->prev,
                     "aws_linked_list_back: node fields unchanged");

    return 0;
}
