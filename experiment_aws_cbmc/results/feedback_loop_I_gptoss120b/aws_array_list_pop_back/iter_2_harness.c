#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Harness for aws_array_list_pop_back */
void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Nondeterministically set a valid length (including zero) */
    size_t max_len = list.current_size / list.item_size;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);
    list.length = len;

    /* Save old state */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    int result = aws_array_list_pop_back(&list);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: element removed, length decreased by 1 */
        assert(list.length == old.length - 1);
        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* Failure: list unchanged (e.g., length was zero) */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 5. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
