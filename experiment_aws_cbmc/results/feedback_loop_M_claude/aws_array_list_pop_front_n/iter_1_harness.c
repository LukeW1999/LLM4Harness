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

    /* 2. Bound n */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* Save a byte from the data for checking (if data is non-null and non-empty) */
    struct store_byte_from_buffer saved_byte;
    bool has_data = (list.data != NULL && list.current_size > 0);

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Assert postconditions */

    /* Fields that must NOT change */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* Length postconditions based on Doxygen:
     * - If n >= old length, list is cleared (length = 0)
     * - Otherwise, length decreases by n
     */
    if (n >= old_list.length) {
        /* List is cleared */
        assert(list.length == 0);
    } else if (n == 0) {
        /* Nothing removed */
        assert(list.length == old_list.length);
    } else {
        /* n elements removed from front */
        assert(list.length == old_list.length - n);
    }

    /* 6. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));
}
