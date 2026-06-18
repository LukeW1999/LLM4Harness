#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Nondeterministic n, constrained to be a valid pop count */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= old.length);
    if (old.item_size != 0) {
        __CPROVER_assume(n <= SIZE_MAX / old.item_size);
    }

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 6. Length post‑condition */
    if (n == 0) {
        assert(list.length == old.length);
    } else {
        assert(list.length == old.length - n);
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
