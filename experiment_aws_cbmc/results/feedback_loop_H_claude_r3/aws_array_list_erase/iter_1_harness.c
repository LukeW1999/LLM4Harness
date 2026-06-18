#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Need at least one element to have a valid erase operation */
    /* index is non-deterministic */
    size_t index = nondet_size_t();

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 3. Assert postconditions */

    /* Unchanged fields regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (< length) */
        assert(index < old.length);
        /* Length decreases by 1 */
        assert(list.length == old.length - 1);
    } else {
        /* On failure: index was out of bounds */
        assert(index >= old.length);
        /* List is unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
    }

    /* 4. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
