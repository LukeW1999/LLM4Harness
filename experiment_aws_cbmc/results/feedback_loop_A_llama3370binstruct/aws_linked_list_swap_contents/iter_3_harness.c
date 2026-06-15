#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list a;
    struct aws_linked_list b;
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* 3. Call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(a.head.next == old_b.head.next);
    assert(a.head.next->prev == &a.head);
    assert(a.tail.prev == old_b.tail.prev);
    assert(a.tail.prev->next == &a.tail);
    assert(b.head.next == old_a.head.next);
    assert(b.head.next->prev == &b.head);
    assert(b.tail.prev == old_a.tail.prev);
    assert(b.tail.prev->next == &b.tail);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
