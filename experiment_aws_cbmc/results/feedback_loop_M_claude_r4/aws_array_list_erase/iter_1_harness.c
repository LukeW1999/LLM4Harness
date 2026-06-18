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
    /* (we test both valid and invalid index paths) */

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Non-deterministic index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (index < old.length) */
        assert(index < old.length);
        /* Length decreases by 1 */
        assert(list.length == old.length - 1);
        /* item_size unchanged */
        assert(list.item_size == old.item_size);
        /* alloc unchanged */
        assert(list.alloc == old.alloc);
        /* current_size unchanged (erase doesn't shrink allocation) */
        assert(list.current_size == old.current_size);
        /* data pointer unchanged */
        assert(list.data == old.data);
    } else {
        /* On failure: index must have been out of bounds */
        assert(index >= old.length);
        /* List must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
