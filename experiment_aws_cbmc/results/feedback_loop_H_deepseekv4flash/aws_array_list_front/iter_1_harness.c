#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

/* Stub comparator: non-deterministically compares two elements */
static int nondet_compare(const void *a, const void *b) {
    (void)a;
    (void)b;
    /* Return non-deterministic comparison result: -1, 0, or 1 */
    int result;
    __CPROVER_assume(result >= -1 && result <= 1);
    return result;
}

void aws_array_list_sort_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling – needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    aws_array_list_sort(&list, nondet_compare);

    /* 4. Assert postconditions */
    assert(aws_array_list_is_valid(&list));

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
}
