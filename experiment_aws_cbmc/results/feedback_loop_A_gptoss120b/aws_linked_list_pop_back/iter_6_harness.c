#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_pop_back_harness(void) {
    /* 1. Initialize the list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Decide whether the list will contain nodes */
    bool has_nodes = nondet_bool();

    if (!has_nodes) {
        /* List stays empty; do not invoke pop_back (precondition requires non‑empty). */
        assert(aws_linked_list_is_valid(&list));
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
        return;
    }

    /* 3. Populate the list with 1 or 2 nodes */
    struct aws_linked_list_node *first = NULL;
    struct aws_linked_list_node *last  = NULL;

    struct aws_linked_list_node *n1 = malloc(sizeof(*n1));
    __CPROVER_assume(n1 != NULL);
    aws_linked_list_push_back(&list, n1);
    first = n1;

    bool add_second = nondet_bool();
    if (add_second) {
        struct aws_linked_list_node *n2 = malloc(sizeof(*n2));
        __CPROVER_assume(n2 != NULL);
        aws_linked_list_push_back(&list, n2);
        last = n2;
    } else {
        last = n1;
    }

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 4. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = old.tail.prev;

    /* 5. Call the function under test (precondition satisfied) */
    struct aws_linked_list_node *removed = aws_linked_list_pop_back(&list);

    /* 6. Post‑conditions */
    assert(aws_linked_list_is_valid(&list));
    assert(removed == last);

    if (old_last->prev == &old.head) {
        /* List became empty */
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* List still has at least one node */
        assert(list.head.next == old.head.next);
    }
}
