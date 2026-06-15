#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_linked_list_push_front_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* node that will be pushed */
    struct aws_linked_list_node *new_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(new_node != NULL);

    /* nondet decide whether the list is initially empty */
    bool list_was_empty = __CPROVER_nondet_bool();

    if (!list_was_empty) {
        /* make the list non‑empty by pushing an existing node */
        struct aws_linked_list_node *existing = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(existing != NULL);
        aws_linked_list_push_back(&list, existing);
    }

    /* save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last  = list.tail.prev;

    /* call function under test */
    aws_linked_list_push_front(&list, new_node);

    /* post‑conditions */
    assert(aws_linked_list_is_valid(&list));

    /* new_node is now the first element */
    assert(list.head.next == new_node);
    assert(new_node->prev == &list.head);
    assert(new_node->next == old_first);

    if (list_was_empty) {
        /* when the list was empty, new_node is also the last element */
        assert(list.tail.prev == new_node);
        assert(new_node->next == &list.tail);
    } else {
        /* when the list was non‑empty, old first element's prev points to new_node */
        assert(old_first->prev == new_node);
        /* tail unchanged */
        assert(list.tail.prev == old_last);
    }

    /* invariant fields of the list */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
