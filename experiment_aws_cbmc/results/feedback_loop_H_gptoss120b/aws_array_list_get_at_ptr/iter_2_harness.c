#include <assert.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_get_at_ptr_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare output pointer and nondeterministic index */
    void *out = NULL;
    void *old_out = out;
    size_t index = nondet_size_t();

    /* 3. Save old state before the call */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, &out, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index must be within bounds and out must point to the element */
        assert(aws_array_list_length(&list) > index);
        assert(out == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: out must remain unchanged */
        assert(out == old_out);
    }

    /* 6. Fields that must not change */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
