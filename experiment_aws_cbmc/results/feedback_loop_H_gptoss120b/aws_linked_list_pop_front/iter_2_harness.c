#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_pop_front_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;
    bool was_empty = (old.head.next == &old.tail);
    struct aws_linked_list_node *old_first = old.head.next;
    struct aws_linked_list_node *old_second = NULL;
    if (!was_empty) {
        old_second = old_first->next;
    }

    /* 3. Call function under test */
    struct aws_linked_list_node *ret = aws_linked_list_pop_front(&list);

    /* 4. Post‑condition: validity invariants must hold */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    if (was_empty) {
        /* List was empty: return value must be NULL and list unchanged */
        assert(ret == NULL);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* List was non‑empty */
        assert(ret == old_first);
        /* Returned node must be detached */
        assert(ret->prev == NULL);
        assert(ret->next == NULL);
        /* New front of the list */
        assert(list.head.next == old_second);
        if (old_second != &old.tail) {
            assert(old_second->prev == &list.head);
        } else {
            /* List became empty after removal */
            assert(list.head.next == &list.tail);
            assert(list.tail.prev == &list.head);
        }
    }
}
