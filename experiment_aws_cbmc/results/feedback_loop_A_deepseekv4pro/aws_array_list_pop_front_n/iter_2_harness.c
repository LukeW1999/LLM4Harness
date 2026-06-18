#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;
    size_t n = nondet_size_t();

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    aws_array_list_pop_front_n(&list, n);

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* Length and data changes */
    if (n >= old.length) {
        /* Cleared case: length must be 0 */
        assert(list.length == 0);
    } else {
        /* Some elements remain */
        assert(list.length == old.length - n);
    }

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
