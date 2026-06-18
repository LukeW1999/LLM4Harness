#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Nondeterministic n (bounded) */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Invariants that must always hold */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 6. Length related post‑conditions */
    if (n >= old.length) {
        /* List cleared (length becomes zero) */
        assert(list.length == 0);
    } else {
        /* Elements removed from the front */
        assert(list.length == old.length - n);
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
