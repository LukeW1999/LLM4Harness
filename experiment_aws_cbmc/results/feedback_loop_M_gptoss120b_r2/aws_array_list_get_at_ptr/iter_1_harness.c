#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare and bound the index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Prepare output pointer */
    void *out = NULL;

    /* 5. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, &out, index);

    /* 6. Post‑condition checks */

    /* Success path */
    if (result == AWS_OP_SUCCESS) {
        /* The index must be within bounds */
        assert(old.length > index);
        /* The returned pointer must point to the correct element */
        assert(out == (void *)((uint8_t *)old.data + (old.item_size * index)));
    } else {
        /* Failure path: index out of bounds */
        assert(old.length <= index);
        /* The output pointer must remain unchanged (still NULL) */
        assert(out == NULL);
    }

    /* 7. Unchanged fields of the list (both paths) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
