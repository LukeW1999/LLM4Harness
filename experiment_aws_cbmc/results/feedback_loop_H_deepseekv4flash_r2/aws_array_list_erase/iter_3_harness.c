#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_erase_harness() {
    /* nondet parameters */
    struct aws_array_list list;
    size_t index = nondet_size_t();

    /* Bound the list to keep proof feasible */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    /* Ensure length > 0 so that index < length is possible */
    __CPROVER_assume(list.length > 0);
    /* Use the default allocator (non-failing) */
    list.alloc = aws_default_allocator();

    /* Save old state for comparison */
    struct aws_array_list old = list;

    /* Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions */
    /* 1. Success path: length decreased by 1, data contents shifted */
    if (result == AWS_OP_SUCCESS) {
        /* length must be old length - 1 */
        assert(list.length == old.length - 1);
        /* list must be valid after operation */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure: index >= old.length; list unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    }

    /* 2. Unchanged fields for both success and failure */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 3. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
