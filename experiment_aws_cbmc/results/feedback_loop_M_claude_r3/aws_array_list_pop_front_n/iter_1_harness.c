#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic n, bounded to prevent state space explosion */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* Save a byte from the data buffer for checking data integrity after the call */
    struct store_byte_from_buffer saved_byte;
    size_t remaining_items_after;
    if (n >= list.length) {
        remaining_items_after = 0;
    } else {
        remaining_items_after = list.length - n;
    }

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Assert postconditions */

    /* Fields that must NOT change */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Changed fields based on Doxygen:
     * - If n >= original length, list is cleared (length = 0)
     * - If n > 0 and n < original length, length decreases by n
     * - If n == 0, nothing changes
     */
    if (n == 0) {
        /* Nothing should change */
        assert(list.length == old.length);
    } else if (n >= old.length) {
        /* List is cleared */
        assert(list.length == 0);
    } else {
        /* n elements removed from front */
        assert(list.length == old.length - n);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
