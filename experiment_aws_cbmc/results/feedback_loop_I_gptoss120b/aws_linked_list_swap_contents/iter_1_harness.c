#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* Harness for aws_linked_list_swap_contents */
void aws_linked_list_swap_contents_harness(void) {
    /* 1. Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Ensure the two lists are distinct (the function is not defined for aliasing) */
    __CPROVER_assume(&a != &b);

    /* 2. Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* 3. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Post‑conditions: contents of a become old contents of b, and vice‑versa */

    /* a reflects old_b */
    if (aws_linked_list_empty(&old_b)) {
        /* a should be empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* head and tail connections are swapped */
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* b reflects old_a */
    if (aws_linked_list_empty(&old_a)) {
        /* b should be empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 5. Unchanged fields – the list structures contain only head/tail nodes,
       whose internal pointers are accounted for above. No other fields exist. */

    /* 6. Validity invariants must hold after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
