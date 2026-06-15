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
void aws_array_list_pop_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Nondeterministically set a valid length */
    size_t max_len = (list.item_size == 0) ? 0 : list.current_size / list.item_size;
    __CPROVER_assume(list.length <= max_len);

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list);

    /* 5. Postconditions */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Success requires the list was non‑empty */
        assert(old.length > 0);
        /* Length decreased by one */
        assert(list.length == old.length - 1);
        /* Unchanged fields */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* Failure occurs only when the list was empty */
        assert(old.length == 0);
        /* List unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }
}
