#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdint.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic n */
    size_t n = nondet_size_t();

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Call function under test (void return) */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Assert postconditions */

    /* Fields that are NEVER modified by this function */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Case 1: n >= original length → clear (length becomes 0) */
    if (n >= old.length) {
        assert(list.length == 0);
    }
    /* Case 2: n == 0 → nothing changes */
    else if (n == 0) {
        assert(list.length == old.length);
    }
    /* Case 3: 0 < n < original length → length reduced by n */
    else {
        assert(list.length == old.length - n);
    }

    /* General postcondition: length is always <= old.length */
    assert(list.length <= old.length);

    /* Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
