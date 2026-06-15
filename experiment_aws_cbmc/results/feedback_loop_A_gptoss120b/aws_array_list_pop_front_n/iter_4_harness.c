#include <aws/common/array_list.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);               /* item_size must be non‑zero */

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Nondeterministic n, bounded to avoid overflow */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= old.length);

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Postconditions */

    /* Length changes */
    if (n == old.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old.length - n);
    }

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
