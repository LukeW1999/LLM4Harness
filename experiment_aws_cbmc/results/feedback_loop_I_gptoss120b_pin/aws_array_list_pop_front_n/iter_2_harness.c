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
    /* Prevent overflow in size calculations */
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

    /* 6. Post‑conditions */
    if (n == 0) {
        /* No change */
        assert(list.length == old.length);
        assert_bytes_match((uint8_t *)list.data,
                           (uint8_t *)old.data,
                           old.length * old.item_size);
    } else {
        size_t new_len = old.length - n;
        assert(list.length == new_len);
        /* Remaining elements are shifted forward */
        assert_bytes_match((uint8_t *)list.data,
                           (uint8_t *)old.data + n * old.item_size,
                           new_len * old.item_size);
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
