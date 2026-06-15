#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_pop_back_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Optionally populate the list with 1 or 2 nodes */
    bool has_nodes = nondet_bool();
    struct aws_linked_list_node *first = NULL;
    struct aws_linked_list_node *last  = NULL;

    if (has_nodes) {
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
    }

    /* 4. Capture the original last node (prev of tail sentinel) */
    struct aws_linked_list_node *old_last = old.tail.prev;

    /* 5. Call the function under test */
    struct aws_linked_list_node *removed = aws_linked_list_pop_back(&list);

    /* 6. Post‑conditions */
    assert(aws_linked_list_is_valid(&list));

    if (!has_nodes) {
        /* List was empty: pop_back returns NULL and list is unchanged */
        assert(removed == NULL);
        assert(list.head.next == old.head.next);
        assert(list.tail.prev == old.tail.prev);
    } else {
        /* List was non‑empty: pop_back returns the former last node */
        assert(removed == last);

        /* New tail.prev must be the node that preceded the removed node */
        assert(list.tail.prev == old_last->prev);

        if (old_last->prev == &old.head) {
            /* The list became empty */
            assert(list.head.next == &list.tail);
            assert(list.tail.prev == &list.head);
        } else {
            /* The list still has at least one node */
            assert(list.head.next == old.head.next);
        }
    }
}
