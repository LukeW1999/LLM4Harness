#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    for (struct aws_linked_list_node *node = aws_linked_list_begin(list);
         node != aws_linked_list_end(list);
         node = aws_linked_list_next(node)) {
        len++;
    }
    return len;
}

void aws_linked_list_insert_after_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* create a non‑empty list */
    struct aws_linked_list_node *first = malloc(sizeof(*first));
    __CPROVER_assume(first != NULL);
    aws_linked_list_node_reset(first);
    aws_linked_list_push_back(&list, first);

    /* add a nondet number (0..4) of additional nodes */
    size_t extra = nondet_uint() % 5;
    for (size_t i = 0; i < extra; ++i) {
        struct aws_linked_list_node *n = malloc(sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_node_reset(n);
        aws_linked_list_push_back(&list, n);
    }

    /* pick a nondet node from the list to serve as 'after' (cannot be the tail sentinel) */
    struct aws_linked_list_node *after = NULL;
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(&list);
         cur != aws_linked_list_end(&list);
         cur = aws_linked_list_next(cur)) {
        if (nondet_bool()) {
            after = cur;
        }
    }
    __CPROVER_assume(after != NULL);               /* list is non‑empty, so after is valid */

    /* allocate a fresh node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);            /* ensure it is not in any list */

    /* capture pre‑state */
    size_t len_before = list_length(&list);
    struct aws_linked_list_node *old_next = after->next;

    /* function under test */
    aws_linked_list_insert_after(after, to_add);

    /* post‑conditions */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    size_t len_after = list_length(&list);
    assert(len_after == len_before + 1);

    assert(to_add->prev == after);
    assert(to_add->next == old_next);
    assert(after->next == to_add);
    assert(old_next->prev == to_add);

    /* frame condition: the list head and tail remain unchanged */
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);

    return 0;
}
