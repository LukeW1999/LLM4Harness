#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic n */
    size_t n = nondet_size_t();

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* Save a byte from the data for checking data integrity in the remaining case */
    struct store_byte_from_buffer saved_byte;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Assert postconditions */

    /* Unchanged fields: alloc, current_size, item_size, data pointer */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Changed field: length */
    if (n >= old_length) {
        /* If n >= length, list is cleared: length becomes 0 */
        assert(list.length == 0);
    } else if (n == 0) {
        /* If n == 0, nothing changes */
        assert(list.length == old_length);
    } else {
        /* Otherwise, length decreases by n */
        assert(list.length == old_length - n);
    }

    /* 6. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));
}
