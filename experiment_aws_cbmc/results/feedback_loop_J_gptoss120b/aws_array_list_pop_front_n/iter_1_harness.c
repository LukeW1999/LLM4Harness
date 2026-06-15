#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic number of elements to pop */
    size_t n = nondet_size_t();

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Post‑condition: length updates correctly */
    if (n >= old.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old.length - n);
    }

    /* 6. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 7. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
