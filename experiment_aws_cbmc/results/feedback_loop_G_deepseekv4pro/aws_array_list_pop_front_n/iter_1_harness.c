#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;

    /* Bound the size to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate and initialize the data member and allocator */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save the old state for postcondition checks */
    struct aws_array_list old = list;

    size_t n = nondet_size_t();

    /* Function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 1. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* 2. Fields that must NOT change */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 3. Length is correctly updated */
    if (n >= old.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old.length - n);
    }
}
