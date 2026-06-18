#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Need at least one element for a meaningful erase, but also test empty case */
    size_t index;

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 3. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (< old length) */
        assert(index < old.length);
        /* Length decreases by 1 */
        assert(list.length == old.length - 1);
        /* item_size unchanged */
        assert(list.item_size == old.item_size);
        /* alloc unchanged */
        assert(list.alloc == old.alloc);
        /* current_size unchanged (pop_back doesn't shrink allocation) */
        assert(list.current_size == old.current_size);
        /* data pointer unchanged */
        assert(list.data == old.data);
    } else {
        /* On failure: index must have been >= length */
        assert(index >= old.length);
        /* List is unchanged on failure */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 4. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
